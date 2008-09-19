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
#include "../Utils/Exception.hh"
#include "../Utils/senfassert.hh"

//#include "FIFORunner.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::detail::FIFORunner::FIFORunner()
    : tasks_ (), next_ (tasks_.end()), watchdogMs_ (1000), watchdogCount_(0), hangCount_ (0)
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
}

prefix_ senf::scheduler::detail::FIFORunner::~FIFORunner()
{
    timer_delete(watchdogId_);
    signal(SIGURG, SIG_DFL);
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

namespace {
    struct NullTask 
        : public senf::scheduler::detail::FIFORunner::TaskInfo
    {
        NullTask() : senf::scheduler::detail::FIFORunner::TaskInfo ("<null>") {}
        void run() {};
    };
}

prefix_ void senf::scheduler::detail::FIFORunner::run()
{
    // This algorithm is carefully adjusted to make it work even when arbitrary tasks are removed
    // from the queue
    // - Before we begin, we add a NullTask to the queue. The only purpose of this node is, to mark
    //   the current end of the queue. The iterator to this node becomes the end iterator of the
    //   range to process
    // - We update the TaskInfo and move it to the end of the queue before calling the callback so
    //   we don't access the TaskInfo if it is removed while the callback is running
    // - We keep the next to-be-processed node in a class variable which is checked and updated
    //   whenever a node is removed.
    NullTask null;
    struct itimerspec timer;
    timer.it_interval.tv_sec = watchdogMs_ / 1000;
    timer.it_interval.tv_nsec = (watchdogMs_ % 1000) * 1000000ul;
    timer.it_value.tv_sec = timer.it_interval.tv_sec;
    timer.it_value.tv_nsec = timer.it_interval.tv_nsec;
    tasks_.push_back(null);
    TaskList::iterator end (TaskList::current(null));
    next_ = tasks_.begin();
    try {
        if (timer_settime(watchdogId_, 0, &timer, 0) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
        while (next_ != end) {
            TaskInfo & task (*next_);
            if (task.runnable_) {
                task.runnable_ = false;
                runningName_ = task.name_;
#           ifdef SENF_DEBUG
                runningBacktrace_ = task.backtrace_;
#           endif
                TaskList::iterator i (next_);
                ++ next_;
                tasks_.splice(tasks_.end(), tasks_, i);
                watchdogCount_ = 1;
                task.run();
            }
            else
                ++ next_;
        }
    }
    catch (...) {
        watchdogCount_ = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 0;
        timer_settime(watchdogId_, 0, &timer, 0);
        tasks_.erase(end);
        next_ = tasks_.end();
        throw;
    }
    watchdogCount_ = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_nsec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_nsec = 0;
    if (timer_settime(watchdogId_, 0, &timer, 0) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("timer_settime()");
    tasks_.erase(end);
    next_ = tasks_.end();
}

prefix_ void senf::scheduler::detail::FIFORunner::watchdog(int, siginfo_t * si, void *)
{
    FIFORunner & runner (*static_cast<FIFORunner *>(si->si_value.sival_ptr));
    if (runner.watchdogCount_ > 0) {
        ++ runner.watchdogCount_;
        if (runner.watchdogCount_ > 2) {
            ++ runner.hangCount_;
            write(1, "\n\n*** Scheduler task hanging: ", 30);
            write(1, runner.runningName_.c_str(), runner.runningName_.size());
            write(1, "\n", 1);
#ifdef SENF_DEBUG
            write(1, "Task was initialized at\n", 24);
            write(1, runner.runningBacktrace_.c_str(), runner.runningBacktrace_.size());
#endif
            write(1, "\n", 1);
        }
    }
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
