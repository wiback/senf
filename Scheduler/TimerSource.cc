// $Id$
//
// Copyright (C) 2009 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief TimerSource non-inline non-template implementation */

#include "TimerSource.hh"
//#include "TimerSource.ih"

// Custom includes
#include "FdEvent.hh"
#ifdef HAVE_TIMERFD
#include <sys/timerfd.h>
#endif

//#include "TimerSource.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerSource

prefix_ senf::scheduler::detail::TimerSource::~TimerSource()
{}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::POSIXTimerSource

prefix_ senf::scheduler::detail::POSIXTimerSource::POSIXTimerSource()
    : timeoutEnabled_ (false), timeout_ (0), signalEnabled_ (false)
{
    if (pipe(timerPipe_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    senf::scheduler::detail::FdManager::instance().set(
        timerPipe_[0], detail::FdManager::EV_READ, this);
    
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
    senf::scheduler::detail::FdManager::instance().remove(timerPipe_[0]);
    close(timerPipe_[0]);
    close(timerPipe_[1]);
}

prefix_ void
senf::scheduler::detail::POSIXTimerSource::timeout(ClockService::clock_type timeout)
{
    if (! timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (timeout_ <= 0)
            timeout_ = 1;
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
    write(static_cast<POSIXTimerSource*>(siginfo->si_value.sival_ptr)->timerPipe_[1], 
          &data, sizeof(data));
}

prefix_ void senf::scheduler::detail::POSIXTimerSource::signal(int events)
{
    char data;
    read(timerPipe_[0], &data, sizeof(data));
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

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::PollTimerSource

prefix_ void senf::scheduler::detail::PollTimerSource::timeout(ClockService::clock_type timeout)
{
    ClockService::clock_type now (ClockService::now());
    int delay (ClockService::in_milliseconds(timeout-now)+1);
    FileDispatcher::instance().timeout(delay<0?0:delay);
}

prefix_ void senf::scheduler::detail::PollTimerSource::notimeout()
{
    FileDispatcher::instance().timeout(-1);
}

prefix_ void senf::scheduler::detail::PollTimerSource::enable()
{}

prefix_ void senf::scheduler::detail::PollTimerSource::disable()
{}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerFDTimerSource

#ifdef HAVE_TIMERFD
prefix_ senf::scheduler::detail::TimerFDTimerSource::TimerFDTimerSource()
    : timerfd_ (-1), timeoutEnabled_ (false), timeout_ (0)
{
    timerfd_ = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timerfd_ < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timerfd_create()");
    senf::scheduler::detail::FdManager::instance().set(
        timerfd_, detail::FdManager::EV_READ, this);
}

prefix_ senf::scheduler::detail::TimerFDTimerSource::~TimerFDTimerSource()
{
    senf::scheduler::detail::FdManager::instance().remove(timerfd_);
    close(timerfd_);
}

prefix_ void
senf::scheduler::detail::TimerFDTimerSource::timeout(ClockService::clock_type timeout)
{
    if (!timeoutEnabled_ || timeout_ != timeout) {
        timeout_ = timeout;
        if (timeout_ <= 0)
            timeout_ = 1;
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
    read(timerfd_, &expirations, sizeof(expirations));
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

///////////////////////////////cc.e////////////////////////////////////////
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
