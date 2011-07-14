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
    TimerSet::iterator i (TimerSet::current(event));
    if (i == timers_.end())
        return;
    FIFORunner::instance().dequeue(&(*i));
    timers_.erase(i);
}

prefix_ void senf::scheduler::detail::TimerDispatcher::prepareRun()
{
    TimerSet::iterator i (timers_.begin());
    TimerSet::iterator const i_end (timers_.end());
    ClockService::clock_type now (FdManager::instance().eventTime());
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
