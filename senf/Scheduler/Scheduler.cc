//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Scheduler non-inline non-template implementation

    \idea Multithreading support: To support multithreading, the
    static member Scheduler::instance() must return a thread-local
    value (that is Scheduler::instance() must allocate one Scheduler
    instance per thread). Another possibility would be to distribute
    the async load unto several threads (one scheduler for multiple
    threads)
 */

#include "Scheduler.hh"
//#include "Scheduler.ih"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    bool terminate_ (false);
    bool running_ (false);
}

prefix_ void senf::scheduler::terminate()
{
    terminate_ = true;
}

prefix_ bool senf::scheduler::running()
{
    return running_;
}

prefix_ void senf::scheduler::yield()
{
    detail::FIFORunner::instance().yield();
}

namespace {

    // We don't want try { } catch(...) { ... throw; } since that will make debugging more
    // difficult: the stack backtrace for an unexpected exception would always end here.
    struct SchedulerScopedInit
    {
        SchedulerScopedInit()
            {
                senf::scheduler::detail::FIFORunner::instance().startWatchdog();
                senf::scheduler::detail::SignalDispatcher::instance().unblockSignals();
                senf::scheduler::detail::TimerDispatcher::instance().enable();
                running_ = true;
            }

        ~SchedulerScopedInit()
            {
                senf::scheduler::detail::TimerDispatcher::instance().disable();
                senf::scheduler::detail::SignalDispatcher::instance().blockSignals();
                senf::scheduler::detail::FIFORunner::instance().stopWatchdog();
                running_ = false;
            }
    };
}

prefix_ void senf::scheduler::process()
{
    SchedulerScopedInit initScheduler;
    terminate_ = false;
    detail::TimerDispatcher::instance().reschedule();
    while(! terminate_ && ! (detail::FdDispatcher::instance().empty() &&
                             detail::TimerDispatcher::instance().empty() &&
                             detail::FileDispatcher::instance().empty() &&
                             detail::IdleEventDispatcher::instance().empty()) ) {
        detail::FdManager::instance().processOnce();
        detail::FileDispatcher::instance().prepareRun();
        detail::EventHookDispatcher::instance().prepareRun();
        detail::TimerDispatcher::instance().prepareRun();
        detail::IdleEventDispatcher::instance().prepareRun();
        detail::FIFORunner::instance().run();
        detail::TimerDispatcher::instance().reschedule();
    }
}

prefix_ void senf::scheduler::restart()
{
    detail::FdManager*            fdm (&detail::FdManager::instance());
    detail::FIFORunner*           ffr (&detail::FIFORunner::instance());
    detail::FdDispatcher*         fdd (&detail::FdDispatcher::instance());
    detail::TimerDispatcher*      tdd (&detail::TimerDispatcher::instance());
    detail::SignalDispatcher*     sdd (&detail::SignalDispatcher::instance());
    detail::FileDispatcher*       fld (&detail::FileDispatcher::instance());
    detail::IdleEventDispatcher*  ied (&detail::IdleEventDispatcher::instance());
    detail::EventHookDispatcher*  eed (&detail::EventHookDispatcher::instance());

    eed->~EventHookDispatcher();
    ied->~IdleEventDispatcher();
    fld->~FileDispatcher();
    sdd->~SignalDispatcher();
    tdd->~TimerDispatcher();
    fdd->~FdDispatcher();
    ffr->~FIFORunner();
    fdm->~FdManager();

    new (fdm) detail::FdManager();
    new (ffr) detail::FIFORunner();
    new (fdd) detail::FdDispatcher();
    new (tdd) detail::TimerDispatcher();
    new (sdd) detail::SignalDispatcher();
    new (fld) detail::FileDispatcher();
    new (ied) detail::IdleEventDispatcher();
    new (eed) detail::EventHookDispatcher();
}

prefix_ bool senf::scheduler::empty()
{
    return detail::FdDispatcher::instance().empty()
        && detail::TimerDispatcher::instance().empty()
        && detail::FileDispatcher::instance().empty()
        && detail::SignalDispatcher::instance().empty()
        && detail::IdleEventDispatcher::instance().empty()
        && detail::EventHookDispatcher::instance().empty();
}

prefix_ void senf::scheduler::hiresTimers()
{
#ifdef HAVE_TIMERFD_CREATE
    if (haveScalableHiresTimers())
        detail::TimerDispatcher::instance().timerSource(
                std::unique_ptr<detail::TimerSource>(new detail::TimerFDTimerSource()));
    else
#endif
        detail::TimerDispatcher::instance().timerSource(
                std::unique_ptr<detail::TimerSource>(new detail::POSIXTimerSource()));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::schedulerLogTimeSource

prefix_ senf::log::time_type senf::scheduler::LogTimeSource::operator()()
    const
{
    return senf::ClockService::in_nanoseconds( scheduler::now());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::BlockSignals

prefix_ senf::scheduler::BlockSignals::BlockSignals(bool initiallyBlocked)
    : blocked_ (false)
{
    ::sigfillset(&allSigs_);
    if (initiallyBlocked)
        block();
}

prefix_ void senf::scheduler::BlockSignals::block()
{
    if (blocked_)
        return;
    ::sigprocmask(SIG_BLOCK, &allSigs_, &savedSigs_);
    blocked_ = true;
}

prefix_ void senf::scheduler::BlockSignals::unblock()
{
    if (!blocked_)
        return;
    ::sigprocmask(SIG_SETMASK, &savedSigs_, 0);
    blocked_ = false;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
