// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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
senf::scheduler::detail::POSIXTimerSource::timeout(ClockService::clock_type timeout)
{
    if (! timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (timeout_ <= ClockService::clock_type(0))
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
    memset(&timer, 0, sizeof(timer));
    if (timeoutEnabled_) {
        timer.it_value.tv_sec = ClockService::in_seconds(timeout_);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(
            timeout_ - ClockService::seconds(timer.it_value.tv_sec));
    }
    if (timer_settime(timerId_, TIMER_ABSTIME, &timer, 0)<0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::PollTimerSource

prefix_ void senf::scheduler::detail::PollTimerSource::timeout(ClockService::clock_type timeout)
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
senf::scheduler::detail::TimerFDTimerSource::timeout(ClockService::clock_type timeout)
{
    if (!timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (timeout_ <= ClockService::clock_type(0))
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
    memset(&timer, 0, sizeof(timer));
    if (timeoutEnabled_) {
        timer.it_value.tv_sec = ClockService::in_seconds(timeout_);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(
            timeout_ - ClockService::seconds(timer.it_value.tv_sec));
    }
    if (timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &timer, 0)<0)
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
