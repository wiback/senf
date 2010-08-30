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
    \brief FIFORunner non-inline non-template implementation */

#include "FIFORunner.hh"
//#include "FIFORunner.ih"

// Custom includes
#include <signal.h>
#include <time.h>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/senfassert.hh>
#ifdef SENF_DEBUG
    #include <execinfo.h>
#endif
#include <senf/config.hh>
#include <stdint.h>
#include <stdio.h>
#include "senf/Utils/IgnoreValue.hh"

//#include "FIFORunner.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::detail::FIFORunner::FIFORunner()
    : tasks_ (), next_ (tasks_.end()), watchdogRunning_ (false), watchdogMs_ (1000),
      watchdogAbort_ (false), watchdogCount_(0), hangCount_ (0), yield_ (false)
{
    struct sigevent ev;
    ::memset(&ev, 0, sizeof(ev));
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGURG;
    ev.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_MONOTONIC, &ev, &watchdogId_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_create()");

    struct sigaction sa;
    ::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = &watchdog;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGURG, &sa, 0) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("sigaction()");

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGURG);
    if (sigprocmask(SIG_UNBLOCK, &mask, 0) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("sigprocmask()");

    tasks_.push_back(highPriorityEnd_);
    tasks_.push_back(normalPriorityEnd_);
}

prefix_ senf::scheduler::detail::FIFORunner::~FIFORunner()
{
    timer_delete(watchdogId_);
    signal(SIGURG, SIG_DFL);
}

prefix_ void senf::scheduler::detail::FIFORunner::startWatchdog()
{
    if (watchdogMs_ > 0) {
        struct itimerspec timer;
        ::memset(&timer, 0, sizeof(timer));

        timer.it_interval.tv_sec = watchdogMs_ / 1000;
        timer.it_interval.tv_nsec = (watchdogMs_ % 1000) * 1000000ul;
        timer.it_value.tv_sec = timer.it_interval.tv_sec;
        timer.it_value.tv_nsec = timer.it_interval.tv_nsec;

        if (timer_settime(watchdogId_, 0, &timer, 0) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");

        watchdogRunning_ = true;
    }
    else
        stopWatchdog();
}

prefix_ void senf::scheduler::detail::FIFORunner::stopWatchdog()
{
    struct itimerspec timer;
    ::memset(&timer, 0, sizeof(timer));

    if (timer_settime(watchdogId_, 0, &timer, 0) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");

    watchdogRunning_ = false;
}

// At the moment, the FIFORunner is not very efficient with many non-runnable tasks since the
// complete list of tasks is traversed on each run().
//
// To optimize this, we woould need a way to find the relative ordering of two tasks in O(1) (at the
// moment, this is an O(N) operation by traversing the list).
//
// One idea is, to give each task an 'order' value. Whenever a task is added at the end, it's order
// value is set to the order value of the last task + 1. Whenever the order value such added exceeds
// some threshold (e.g. 2^31 -1 or some such), the task list is traversed from beginning to end to
// assign new consecutive order values. This O(N) operation is so seldom, that it is amortized over
// a very long time.
//
// With this value at hand, we can do several optimizations: One idea would be the following: The
// runnable set always has two types of tasks: There are tasks, which are heavily active and are
// signaled constantly and other tasks which lie dormant most of the time. Those dormant tasks will
// end up at the beginning of the task queue.
//
// With the above defined 'ordering' field available, we can manage an iterator pointing to the
// first and the last runnable task. This will often help a lot since the group of runnable tasks
// will mostly be localized to the end of the queue. only occasionally one of the dormant tasks will
// be runnable. This additional traversal time will be amortized over a larger time.

prefix_ void senf::scheduler::detail::FIFORunner::dequeue(TaskInfo * task)
{
    TaskList::iterator i (TaskList::current(*task));
    if (next_ == i)
        ++next_;
    tasks_.erase(i);
}

prefix_ void senf::scheduler::detail::FIFORunner::run()
{
    for(;;) {
        TaskList::iterator f (tasks_.begin());
        TaskList::iterator l (TaskList::current(highPriorityEnd_));
        run(f, l);
        if (yield_) {
            yield_ = false;
            continue;
        }

        f = l; ++f;
        l = TaskList::current(normalPriorityEnd_);
        run(f, l);
        if (yield_) {
            yield_ = false;
            continue;
        }

        f = l; ++f;
        l = tasks_.end();
        run(f, l);
        if (yield_) {
            yield_ = false;
            continue;
        }
        break;
    }
}

prefix_ void senf::scheduler::detail::FIFORunner::run(TaskList::iterator f, TaskList::iterator l)
{
    if (f == l)
        // We'll have problems inserting NullTask between f and l below, so just explicitly bail out
        return;

    // This algorithm is carefully adjusted to make it work even when arbitrary tasks are removed
    // from the queue
    // - Before we begin, we add a NullTask to the queue. The only purpose of this node is, to mark
    //   the current end of the queue. The iterator to this node becomes the end iterator of the
    //   range to process
    // - We update the TaskInfo and move it to the next queue Element before calling the callback so
    //   we don't access the TaskInfo if it is removed while the callback is running
    // - We keep the next to-be-processed node in a class variable which is checked and updated
    //   whenever a node is removed.

    NullTask null;
    tasks_.insert(l, null);
    TaskList::iterator end (TaskList::current(null));
    next_ = f;

    // Would prefer to use ScopeExit+boost::lambda here instead of try but profiling has shown that
    // to be to costly here

    try {
        while (next_ != end) {
            TaskInfo & task (*next_);
            if (task.runnable_) {
                task.runnable_ = false;
                runningName_ = task.name();
    #       ifdef SENF_DEBUG
                runningBacktrace_ = task.backtrace_;
    #       endif
                TaskList::iterator i (next_);
                ++ next_;
                tasks_.splice(l, tasks_, i);
                watchdogCount_ = 1;
                yield_ = false;
                task.run();
                if (yield_)
                    return;
            }
            else
                ++ next_;
        }
        watchdogCount_ = 0;
        next_ = l;
    }
    catch (...) {
        watchdogCount_ = 0;
        next_ = l;
        throw;
    }
}

prefix_ senf::scheduler::detail::FIFORunner::TaskList::iterator
senf::scheduler::detail::FIFORunner::priorityEnd(TaskInfo::Priority p)
{
    switch (p) {
    case TaskInfo::PRIORITY_LOW :
        return tasks_.end();
    case TaskInfo::PRIORITY_NORMAL :
        return TaskList::current(normalPriorityEnd_);
    case TaskInfo::PRIORITY_HIGH :
        return TaskList::current(highPriorityEnd_);
    }
    return tasks_.begin();
}

prefix_ void senf::scheduler::detail::FIFORunner::watchdog(int, siginfo_t * si, void *)
{
    FIFORunner & runner (*static_cast<FIFORunner *>(si->si_value.sival_ptr));
    if (runner.watchdogCount_ > 0) {
        ++ runner.watchdogCount_;
        if (runner.watchdogCount_ > 2) {
            ++ runner.hangCount_;
            runner.watchdogError();
        }
    }
}

prefix_ void senf::scheduler::detail::FIFORunner::watchdogError()
{
    static char const hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                'a', 'b', 'c', 'd', 'e', 'f' };
    static void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];

    // We don't care if the write commands below fail, we just give our best to inform the user
    senf::IGNORE( write(1, "\n\n*** Scheduler task hanging (pid ",34) );
    static char pid[7];
    ::snprintf(pid, 7, "%6d", ::getpid());
    pid[6] = 0;
    senf::IGNORE( write(1, pid, 6) );
    senf::IGNORE( write(1, "): ", 3) );
    senf::IGNORE( write(1, runningName_.c_str(), runningName_.size()) );
    senf::IGNORE( write(1, " at\n ", 3) );
#ifdef SENF_DEBUG
    unsigned nEntries( ::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS) );
    for (unsigned i (0); i < nEntries; ++i) {
        senf::IGNORE( write(1, " 0x", 3) );
        for (unsigned j (sizeof(void*)); j > 0; --j) {
            uintptr_t v ( reinterpret_cast<uintptr_t>(entries[i]) >> (8*(j-1)) );
            senf::IGNORE( write(1, &(hex[ (v >> 4) & 0x0f ]), 1) );
            senf::IGNORE( write(1, &(hex[ (v     ) & 0x0f ]), 1) );
        }
    }
#endif
    senf::IGNORE( write(1, "\n", 1) );

#ifdef SENF_DEBUG
    senf::IGNORE( write(1, "Task was initialized at\n", 24) );
    senf::IGNORE( write(1, runningBacktrace_.c_str(), runningBacktrace_.size()) );
#endif
    senf::IGNORE( write(1, "\n", 1) );
    if (watchdogAbort_)
        assert(false);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FIFORunner.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
