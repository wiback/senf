//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TimerSource non-inline non-template implementation */

#include "TimerSource.hh"
//#include "TimerSource.ih"

// Custom includes
#include "IdleEvent.hh"
#ifdef HAVE_TIMERFD_CREATE
#include TIMERFD_H_PATH
#endif
#include "senf/Utils/IgnoreValue.hh"

//#include "TimerSource.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerSource

prefix_ senf::scheduler::detail::TimerSource::~TimerSource()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::POSIXTimerSource

prefix_ senf::scheduler::detail::POSIXTimerSource::POSIXTimerSource()
    : timeoutEnabled_ (false), timeout_ (0), signalEnabled_ (false)
{
    if (pipe(timerPipe_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    FdManager::instance().set( timerPipe_[0], FdManager::EV_READ, this);

    sigemptyset(&sigSet_);
    sigaddset(&sigSet_, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigSet_, 0);

    struct sigaction act;
    act.sa_sigaction = &sigHandler;
    act.sa_mask = sigSet_;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    if (sigaction(SIGALRM, &act, 0) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("sigaction()");

    struct sigevent ev;
    ::memset(&ev, 0, sizeof(ev));
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGALRM;
    ev.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_MONOTONIC, &ev, &timerId_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_create()");
}

prefix_ senf::scheduler::detail::POSIXTimerSource::~POSIXTimerSource()
{
    timer_delete(timerId_);
    ::signal(SIGALRM, SIG_IGN);
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    FdManager::instance().remove(timerPipe_[0]);
    close(timerPipe_[0]);
    close(timerPipe_[1]);
}

prefix_ void
senf::scheduler::detail::POSIXTimerSource::timeout(ClockService::clock_type const & timeout)
{
    if (! timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (SENF_UNLIKELY(timeout_ <= ClockService::clock_type(0)))
            timeout_ = ClockService::clock_type(1);
        timeoutEnabled_ = true;
        reschedule();
    }
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::notimeout()
{
    if (timeoutEnabled_) {
        timeoutEnabled_ = false;
        reschedule();
    }
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::enable()
{
    if (! signalEnabled_) {
        signalEnabled_ = true;
        sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    }
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::disable()
{
    if (signalEnabled_) {
        signalEnabled_ = false;
        sigprocmask(SIG_BLOCK, &sigSet_, 0);
    }
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::sigHandler(int,
                                                                   ::siginfo_t * siginfo,
                                                                   void *)
{
    if (siginfo->si_value.sival_ptr == 0)
        return;
    static char data = '\xD0';
    // If the write fails there's not much we can do anyways ...
    senf::IGNORE( write(static_cast<POSIXTimerSource*>(siginfo->si_value.sival_ptr)->timerPipe_[1],
                        &data, sizeof(data)) );
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::signal(int events)
{
    char data;
    // This should never fail since we are reading a single character from a signaled
    // filedescriptor
    senf::IGNORE( read(timerPipe_[0], &data, sizeof(data)) );
    timeoutEnabled_ = false;
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::reschedule()
{
    struct itimerspec timer;
    memset(&timer.it_interval, 0, sizeof(timer.it_interval));
    if (timeoutEnabled_) {
        timer.it_value.tv_sec = ClockService::in_seconds(timeout_);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(timeout_) % 1000000000LL;
    }
    if (SENF_UNLIKELY(timer_settime(timerId_, TIMER_ABSTIME, &timer, 0)<0))
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::PollTimerSource

prefix_ void senf::scheduler::detail::PollTimerSource::timeout(ClockService::clock_type const & timeout)
{
    ClockService::clock_type now (ClockService::now());
    int delay (ClockService::in_milliseconds(timeout-now)+1);
    IdleEventDispatcher::instance().timeout(delay<0?0:delay);
}

prefix_ void senf::scheduler::detail::PollTimerSource::notimeout()
{
    IdleEventDispatcher::instance().timeout(-1);
}

prefix_ void senf::scheduler::detail::PollTimerSource::enable()
{}

prefix_ void senf::scheduler::detail::PollTimerSource::disable()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerFDTimerSource

#ifdef HAVE_TIMERFD_CREATE
prefix_ senf::scheduler::detail::TimerFDTimerSource::TimerFDTimerSource()
    : timerfd_ (-1), timeoutEnabled_ (false), timeout_(0)
{
    timerfd_ = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timerfd_ < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timerfd_create()");
    FdManager::instance().set( timerfd_, FdManager::EV_READ, this);
}

prefix_ senf::scheduler::detail::TimerFDTimerSource::~TimerFDTimerSource()
{
    FdManager::instance().remove(timerfd_);
    close(timerfd_);
}

prefix_ void
senf::scheduler::detail::TimerFDTimerSource::timeout(ClockService::clock_type const & timeout)
{
    if (!timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (SENF_UNLIKELY(timeout_ <= ClockService::clock_type(0)))
            timeout_ = ClockService::clock_type(1);
        timeoutEnabled_ = true;
        reschedule();
    }
}

prefix_ void senf::scheduler::detail::TimerFDTimerSource::notimeout()
{
    if (timeoutEnabled_) {
        timeoutEnabled_ = false;
        reschedule();
    }
}

prefix_ void senf::scheduler::detail::TimerFDTimerSource::enable()
{}

prefix_ void senf::scheduler::detail::TimerFDTimerSource::disable()
{}

namespace {

    struct TimerFdCheck
    {
        TimerFdCheck();
        bool timerFdOk;
    };

    TimerFdCheck::TimerFdCheck()
        : timerFdOk (false)
    {
        int fd (timerfd_create(CLOCK_MONOTONIC, 0));
        if (fd == -1) {
            if (errno != EINVAL)
                SENF_THROW_SYSTEM_EXCEPTION("timerfd_create()");
        }
        else {
            timerFdOk = true;
            close(fd);
        }
    }

}

prefix_ bool senf::scheduler::detail::TimerFDTimerSource::haveTimerFD()
{
    static TimerFdCheck check;
    return check.timerFdOk;
}

prefix_ void senf::scheduler::detail::TimerFDTimerSource::signal(int events)
{
    uint64_t expirations (0);
    // We ignore the return value since we ignore the value read anyways
    senf::IGNORE( read(timerfd_, &expirations, sizeof(expirations)) );
}

prefix_ void senf::scheduler::detail::TimerFDTimerSource::reschedule()
{
    struct itimerspec timer;
    memset(&timer.it_interval, 0, sizeof(timer.it_interval));
    if (timeoutEnabled_) {
        timer.it_value.tv_sec  = ClockService::in_seconds(timeout_);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(timeout_) % 1000000000LL;
    }
    if (SENF_UNLIKELY(timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &timer, 0)<0))
        SENF_THROW_SYSTEM_EXCEPTION("timerfd_settime()");
}
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TimerSource.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
