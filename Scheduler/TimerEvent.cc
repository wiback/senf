// $Id$
//
// Copyright (C) 2008 
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
    \brief TimerDispatcher non-inline non-template implementation */

#include "TimerEvent.hh"
#include "TimerEvent.ih"

// Custom includes

//#include "TimerEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::detail::TimerDispatcher::TimerDispatcher()
    : blocked_ (true)
{
    if (pipe(timerPipe_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    senf::scheduler::detail::FdManager::instance().set(timerPipe_[0], detail::FdManager::EV_READ, this);

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

prefix_ senf::scheduler::detail::TimerDispatcher::~TimerDispatcher()
{
    TimerSet::iterator i (timers_.begin());
    TimerSet::iterator const i_end (timers_.end());
    for (; i != i_end; ++i)
        senf::scheduler::detail::FIFORunner::instance().dequeue(&(*i));

    timer_delete(timerId_);
    ::signal(SIGALRM, SIG_IGN);
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    senf::scheduler::detail::FdManager::instance().remove(timerPipe_[0]);
    close(timerPipe_[0]);
    close(timerPipe_[1]);
}

void senf::scheduler::detail::TimerDispatcher::add(TimerEvent & event)
{
    TimerSet::iterator i (timers_.insert(event));
    senf::scheduler::detail::FIFORunner::instance().enqueue(&(*i));
    if (! blocked_)
        reschedule();
}

prefix_ void senf::scheduler::detail::TimerDispatcher::remove(TimerEvent & event)
{
    TimerSet::iterator i (TimerSet::current(event));
    if (i == timers_.end())
        return;
    senf::scheduler::detail::FIFORunner::instance().dequeue(&(*i));
    timers_.erase(i);
    if (! blocked_)
        reschedule();
}

prefix_ void senf::scheduler::detail::TimerDispatcher::blockSignals()
{
    if (blocked_) 
        return;
    sigprocmask(SIG_BLOCK, &sigSet_, 0);
    blocked_ = true;
}

prefix_ void senf::scheduler::detail::TimerDispatcher::unblockSignals()
{
    if (! blocked_)
        return;
    reschedule();
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    blocked_ = false;
}

prefix_ void senf::scheduler::detail::TimerDispatcher::signal(int events)
{
    siginfo_t info;
    if (read(timerPipe_[0], &info, sizeof(info)) < int(sizeof(info)))
        return;
    TimerSet::iterator i (timers_.begin());
    TimerSet::iterator const i_end (timers_.end());
    ClockService::clock_type now (senf::scheduler::detail::FdManager::instance().eventTime());
    for (; i != i_end && i->timeout_ <= now ; ++i)
        i->setRunnable();
}

prefix_ void senf::scheduler::detail::TimerDispatcher::sigHandler(int signal, 
                                                                  ::siginfo_t * siginfo,
                                                                  void *)
{
    // The manpage says, si_signo is unused in linux so we set it here
    siginfo->si_signo = signal; 
    // We can't do much on error anyway so we ignore errors here
    if (siginfo->si_value.sival_ptr == 0)
        return;
    write(static_cast<TimerDispatcher*>(siginfo->si_value.sival_ptr)->timerPipe_[1], 
          siginfo, sizeof(*siginfo));
}

prefix_ void senf::scheduler::detail::TimerDispatcher::reschedule()
{
    struct itimerspec timer;
    memset(&timer, 0, sizeof(timer));
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_nsec = 0;
    if (timers_.empty()) {
        SENF_LOG( (senf::log::VERBOSE)("Timer disabled") );
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 0;
    }
    else {
        ClockService::clock_type next (timers_.begin()->timeout_);
        if (next <= 0)
            next = 1;
        timer.it_value.tv_sec = ClockService::in_seconds(next);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(
            next - ClockService::seconds(timer.it_value.tv_sec));
        SENF_LOG( (senf::log::VERBOSE)("Next timeout scheduled @" << timer.it_value.tv_sec << "." 
                   << std::setw(9) << std::setfill('0') << timer.it_value.tv_nsec) );
    }
    if (timer_settime(timerId_, TIMER_ABSTIME, &timer, 0)<0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerDispatcher::TimerEvent

prefix_ void senf::scheduler::TimerEvent::run()
{
    disable();
    cb_();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TimerEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
