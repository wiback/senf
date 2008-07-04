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

#include "TimerDispatcher.hh"
//#include "TimerDispatcher.ih"

// Custom includes

//#include "TimerDispatcher.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

unsigned senf::scheduler::TimerDispatcher::useCount_ (0);

prefix_ senf::scheduler::TimerDispatcher::TimerDispatcher(FdManager & manager,
                                                          FIFORunner & runner)
    : manager_ (manager), runner_ (runner), lastId_ (0), blocked_ (true)
{
    if (pipe(timerPipe_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    manager_.set(timerPipe_[0], FdManager::EV_READ, this);

    sigemptyset(&sigSet_);
    sigaddset(&sigSet_, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigSet_, 0);

    if (useCount_ == 0) {
        struct sigaction act;
        act.sa_sigaction = &sigHandler;
        act.sa_mask = sigSet_;
        act.sa_flags = SA_SIGINFO | SA_RESTART;
        if (sigaction(SIGALRM, &act, 0) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("sigaction()");
    }

    struct sigevent ev;
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGALRM;
    ev.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_MONOTONIC, &ev, &timerId_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_create()");
    
    ++ useCount_;
}

prefix_ senf::scheduler::TimerDispatcher::~TimerDispatcher()
{
    -- useCount_;

    TimerMap::iterator i (timers_.begin());
    TimerMap::iterator const i_end (timers_.end());
    for (; i != i_end; ++i)
        runner_.dequeue(&(i->second));

    timer_delete(timerId_);
    if (useCount_ == 0)
        ::signal(SIGALRM, SIG_IGN);
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    manager_.remove(timerPipe_[0]);
    close(timerPipe_[0]);
    close(timerPipe_[1]);
}

prefix_ senf::scheduler::TimerDispatcher::timer_id
senf::scheduler::TimerDispatcher::add(ClockService::clock_type timeout, Callback const & cb)
{
    while (timerIdIndex_.find(++lastId_) != timerIdIndex_.end()) ;
    TimerMap::iterator i (timers_.insert(std::make_pair(timeout, TimerEvent(lastId_, cb, *this))));
    timerIdIndex_.insert(std::make_pair(lastId_, i));
    runner_.enqueue(&(i->second));
    if (! blocked_)
        reschedule();
    return lastId_;
}

prefix_ void senf::scheduler::TimerDispatcher::remove(timer_id id)
{
    TimerIdMap::iterator i (timerIdIndex_.find(id));
    if (i == timerIdIndex_.end())
        return;
    runner_.dequeue(&(i->second->second));
    timers_.erase(i->second);
    timerIdIndex_.erase(i);
    if (! blocked_)
        reschedule();
}

prefix_ void senf::scheduler::TimerDispatcher::blockSignals()
{
    if (blocked_) 
        return;
    sigprocmask(SIG_BLOCK, &sigSet_, 0);
    blocked_ = true;
}

prefix_ void senf::scheduler::TimerDispatcher::unblockSignals()
{
    if (! blocked_)
        return;
    reschedule();
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    blocked_ = false;
}

prefix_ void senf::scheduler::TimerDispatcher::signal(int events)
{
    siginfo_t info;
    if (read(timerPipe_[0], &info, sizeof(info)) < int(sizeof(info)))
        return;

    TimerMap::iterator i (timers_.begin());
    TimerMap::iterator const i_end (timers_.end());
    ClockService::clock_type now (ClockService::now());
    for (; i != i_end && i->first <= now ; ++i)
        i->second.runnable = true;
}

prefix_ void senf::scheduler::TimerDispatcher::sigHandler(int signal, ::siginfo_t * siginfo,
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

prefix_ void senf::scheduler::TimerDispatcher::reschedule()
{
    struct itimerspec timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_nsec = 0;
    if (timers_.empty()) {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 0;
    }
    else {
        ClockService::clock_type next (timers_.begin()->first);
        timer.it_value.tv_sec = ClockService::in_seconds(next);
        timer.it_value.tv_nsec = ClockService::in_nanoseconds(
            next - ClockService::seconds(timer.it_value.tv_sec));
    }
    if (timer_settime(timerId_, TIMER_ABSTIME, &timer, 0)<0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TimerDispatcher.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
