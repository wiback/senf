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
#include <senf/Utils/senflikely.hh>
#include "ConsoleDir.hh"

//#include "FIFORunner.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


//
// Our default watchdog hanging output handler (write to stdout)
//
static void watchdogDefaultOutput(std::string const & taskName, std::string const & checkPointName, std::string const & backTrace,
                                        unsigned periodInMs, unsigned numPeriods)
{
    senf::IGNORE( write(1, "*** Scheduler task hanging (pid ", 32) );
    static char pid[7];
    ::snprintf(pid, 7, "%6d", ::getpid());
    pid[6] = 0;
    senf::IGNORE( write(1, pid, 6) );
    senf::IGNORE( write(1, "): ", 3) );
    senf::IGNORE( write(1, taskName.c_str(), taskName.size()) );
    if (!checkPointName.empty()) {
        senf::IGNORE( write(1, "  Last Checkpoint: ", 19));
        senf::IGNORE( write(1, checkPointName.c_str(), checkPointName.size()) );
    }
    senf::IGNORE( write(1, "\n", 1) );

    if (!backTrace.empty()) {
        senf::IGNORE( write(1, "Task was initialized at\n", 24) );
        senf::IGNORE( write(1, backTrace.c_str(), backTrace.size()) );
        senf::IGNORE( write(1, "\n", 1) );
    }
}


//
// FIFO Runner 
//
prefix_ senf::scheduler::detail::FIFORunner::FIFORunner()
    : tasks_ (), next_ (tasks_.end()), watchdogRunning_ (false), watchdogMs_ (1000),
      watchdogAbort_ (false), watchdogCheckpoint_(nullptr), runningTask_(nullptr),
      watchdogCallback_(::watchdogDefaultOutput),
      watchdogCount_(0), hangCount_ (0), yield_ (false)
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
        runningTask_ = nullptr;
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
        if (SENF_UNLIKELY(yield_)) {
            yield_ = false;
            continue;
        }

        f = l; ++f;
        l = TaskList::s_iterator_to(normalPriorityEnd_);
        run(f, l);
        if (SENF_UNLIKELY(yield_)) {
            yield_ = false;
            continue;
        }

        f = l; ++f;
        l = tasks_.end();
        run(f, l);
        if (SENF_UNLIKELY(yield_)) {
            yield_ = false;
            continue;
        }
        break;
    }
}

prefix_ void senf::scheduler::detail::FIFORunner::run(TaskList::iterator f, TaskList::iterator l)
{
    if (SENF_UNLIKELY(f == l))
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
        while (SENF_LIKELY(next_ != end)) {
            runningTask_ = &(*next_);
            if (runningTask_->runnable_) {
                runningTask_->runnable_ = false;
                TaskList::iterator i (next_);
                ++ next_;
                tasks_.splice(l, tasks_, i);
                watchdogCount_ = 1;
                yield_ = false;
                runningTask_->run();
                if (SENF_UNLIKELY(yield_)) {
                    tasks_.erase(end);
                    return;
                }
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
    std::string taskName (runningTask_ ? runningTask_->name() : runningName_);
    std::string checkpointName (watchdogCheckpoint_ ? std::string(watchdogCheckpoint_) : std::string("")); 
#ifdef SENF_BACKTRACE
    std::string backTrace (runningTask_ ? runningTask_->backtrace_ : runningBacktrace_);
#else
    std::string backTrace ("");
#endif

    watchdogCallback_(taskName, checkpointName, backTrace, watchdogMs_, watchdogCount_ -1);
    
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
