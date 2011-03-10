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
