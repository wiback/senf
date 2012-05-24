// $Id$
//
// Copyright (C) 2008
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
    \brief FIFORunner non-inline non-template implementation */

#include "FIFORunner.hh"
//#include "FIFORunner.ih"

// Custom includes
#include <signal.h>
#include <time.h>
#include <cassert>
#include <senf/config.hh>
#ifdef SENF_BACKTRACE
    #include <execinfo.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <senf/Utils/Exception.hh>
#include "senf/Utils/IgnoreValue.hh"
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include "ConsoleDir.hh"

//#include "FIFORunner.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::scheduler::detail::FIFORunner::FIFORunner()
    : tasks_ (), next_ (tasks_.end()), watchdogRunning_ (false), watchdogMs_ (1000),
      watchdogAbort_ (false), runningTask_(NULL), watchdogCount_(0), hangCount_ (0), yield_ (false)
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

#ifndef SENF_DISABLE_CONSOLE
    namespace fty = console::factory;
    consoleDir().add("abortOnWatchdocTimeout", fty::Command(
            SENF_MEMBINDFNP( bool, FIFORunner, abortOnTimeout, () const ))
        .doc("Get current watchdog abort on event status.") );
    consoleDir().add("abortOnWatchdocTimeout", fty::Command(
                SENF_MEMBINDFNP( void, FIFORunner, abortOnTimeout, (bool) ))
        .doc("Enable/disable abort on watchdog event.") );
    consoleDir().add("watchdogTimeout", fty::Command(
            SENF_MEMBINDFNP( unsigned, FIFORunner, taskTimeout, () const ))
        .doc("Get current watchdog timeout in milliseconds") );
    consoleDir().add("watchdogTimeout", fty::Command(
            SENF_MEMBINDFNP( void, FIFORunner, taskTimeout, (unsigned) ))
        .doc("Set watchdog timeout to in milliseconds\n"
                "Setting the watchdog timeout to 0 will disable the watchdog.") );
    consoleDir().add("watchdogEvents", fty::Command(membind( &FIFORunner::hangCount, this))
        .doc("Get number of occurred watchdog events.\n"
                "Calling this method will reset the counter to 0") );
#endif
}

prefix_ senf::scheduler::detail::FIFORunner::~FIFORunner()
{
    timer_delete(watchdogId_);
    signal(SIGURG, SIG_DFL);

#ifndef SENF_DISABLE_CONSOLE
    consoleDir().remove("abortOnWatchdocTimeout");
    consoleDir().remove("watchdogTimeout");
    consoleDir().remove("watchdogEvents");
#endif
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
// To optimize this, we would need a way to find the relative ordering of two tasks in O(1) (at the
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
    TaskList::iterator i (TaskList::s_iterator_to(*task));
    if (next_ == i)
        ++next_;
    if (runningTask_ == task) {
        runningTask_ = NULL;
        runningName_ = task->name();
#ifdef SENF_BACKTRACE
        runningBacktrace_ = task->backtrace_;
#endif
    }
    tasks_.erase(i);
}

prefix_ void senf::scheduler::detail::FIFORunner::run()
{
    for (;;) {
        TaskList::iterator f (tasks_.begin());
        TaskList::iterator l (TaskList::s_iterator_to(highPriorityEnd_));
        run(f, l);
        if (yield_) {
            yield_ = false;
            continue;
        }

        f = l; ++f;
        l = TaskList::s_iterator_to(normalPriorityEnd_);
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

    TaskList::iterator end (tasks_.insert(l, queueEnd_));
    next_ = f;

    // Would prefer to use ScopeExit+boost::lambda here instead of try but profiling has shown that
    // to be to costly here

    try {
        while (next_ != end) {
            runningTask_ = &(*next_);
            if (runningTask_->runnable_) {
                runningTask_->runnable_ = false;
                TaskList::iterator i (next_);
                ++ next_;
                tasks_.splice(l, tasks_, i);
                watchdogCount_ = 1;
                yield_ = false;
                runningTask_->run();
                if (yield_)
                    return;
            }
            else
                ++ next_;
        }
        watchdogCount_ = 0;
        next_ = l;
        tasks_.erase(end);
    }
    catch (...) {
        watchdogCount_ = 0;
        next_ = l;
        tasks_.erase(end);
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
        return TaskList::s_iterator_to(normalPriorityEnd_);
    case TaskInfo::PRIORITY_HIGH :
        return TaskList::s_iterator_to(highPriorityEnd_);
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
    // We don't care if the write commands below fail, we just give our best to inform the user
    senf::IGNORE( write(1, "\n\n*** Scheduler task hanging (pid ",34) );
    static char pid[7];
    ::snprintf(pid, 7, "%6d", ::getpid());
    pid[6] = 0;
    senf::IGNORE( write(1, pid, 6) );
    senf::IGNORE( write(1, "): ", 3) );
    if (runningTask_)
        senf::IGNORE( write(1, runningTask_->name().c_str(), runningTask_->name().size()) );
    else
        senf::IGNORE( write(1, runningName_.c_str(), runningName_.size()) );
/*    senf::IGNORE( write(1, " at\n ", 3) );
#ifdef SENF_BACKTRACE
    static char const hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                'a', 'b', 'c', 'd', 'e', 'f' };
    static void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];
    int nEntries( ::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS) );
    for (int i=0; i < nEntries; ++i) {
        senf::IGNORE( write(1, " 0x", 3) );
        for (unsigned j (sizeof(void*)); j > 0; --j) {
            uintptr_t v ( reinterpret_cast<uintptr_t>(entries[i]) >> (8*(j-1)) );
            senf::IGNORE( write(1, &(hex[ (v >> 4) & 0x0f ]), 1) );
            senf::IGNORE( write(1, &(hex[ (v     ) & 0x0f ]), 1) );
        }
    }
#endif*/
    senf::IGNORE( write(1, "\n", 1) );

#ifdef SENF_BACKTRACE
    senf::IGNORE( write(1, "Task was initialized at\n", 24) );
    if (runningTask_)
        senf::IGNORE( write(1, runningTask_->backtrace_.c_str(), runningTask_->backtrace_.size()) );
    else
        senf::IGNORE( write(1, runningBacktrace_.c_str(), runningBacktrace_.size()) );
    senf::IGNORE( write(1, "\n", 1) );
#endif
    if (watchdogAbort_)
        assert(false);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
