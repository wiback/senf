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
    \brief TimerDispatcher non-inline non-template implementation */

#include "TimerEvent.hh"
#include "TimerEvent.ih"

// Custom includes
#include <sstream>

//#include "TimerEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::scheduler::detail::TimerDispatcher::TimerDispatcher()
    : source_ (new PollTimerSource())
{}

prefix_ senf::scheduler::detail::TimerDispatcher::~TimerDispatcher()
{
    TimerSet::iterator i (timers_.begin());
    TimerSet::iterator const i_end (timers_.end());
    for (; i != i_end; ++i)
        FIFORunner::instance().dequeue(&(*i));
}

void senf::scheduler::detail::TimerDispatcher::add(TimerEvent & event)
{
    TimerSet::iterator i (timers_.insert(event));
    FIFORunner::instance().enqueue(&(*i));
}

prefix_ void senf::scheduler::detail::TimerDispatcher::remove(TimerEvent & event)
{
    TimerSet::iterator i (TimerSet::s_iterator_to(event));
    if (i == timers_.end())
        return;
    FIFORunner::instance().dequeue(&(*i));
    timers_.erase(i);
}

prefix_ void senf::scheduler::detail::TimerDispatcher::prepareRun()
{
    TimerSet::iterator i (timers_.begin());
    TimerSet::iterator const i_end (timers_.end());
    ClockService::clock_type const & now (FdManager::instance().eventTime());
    for (; i != i_end && i->timeout_ <= now ; ++i)
        i->setRunnable();
}

prefix_ void senf::scheduler::detail::TimerDispatcher::reschedule()
{
    if (timers_.empty())
        source_->notimeout();
    else
        source_->timeout(timers_.begin()->timeout_);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::TimerDispatcher::TimerEvent

prefix_ void senf::scheduler::TimerEvent::v_run()
{
    disable();
    cb_();
}

prefix_ char const * senf::scheduler::TimerEvent::v_type()
    const
{
    return "tm";
}

prefix_ std::string senf::scheduler::TimerEvent::v_info()
    const
{
    std::stringstream ss;
    ss.imbue( std::locale(ss.getloc(),
                          new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f-0000")) );
    ss << "expire " << ClockService::abstime(timeout_);
    return ss.str();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
