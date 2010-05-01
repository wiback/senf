// $Id$
//
// Copyright (C) 2009
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
    \brief IdleEvent non-inline non-template implementation */

#include "IdleEvent.hh"
#include "IdleEvent.ih"

// Custom includes

//#include "IdleEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::IdleEvent

prefix_ void senf::scheduler::IdleEvent::v_run()
{
    cb_();
}

prefix_ char const * senf::scheduler::IdleEvent::v_type()
    const
{
    return "id";
}

prefix_ std::string senf::scheduler::IdleEvent::v_info()
    const
{
    return "";
}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::IdleEventDispatcher

prefix_ void senf::scheduler::detail::IdleEventDispatcher::remove(IdleEvent & event)
{
    EventList::iterator i (EventList::current(event));
    if (i == events_.end())
        return;
    FIFORunner::instance().dequeue(&event);
    events_.erase(i);
    if (events_.empty())
        detail::FileDispatcher::instance().timeout(managerTimeout_);
}

prefix_ void senf::scheduler::detail::IdleEventDispatcher::prepareRun()
{
    for (EventList::iterator i (events_.begin()), i_end (events_.end()); i != i_end; ++i)
        i->setRunnable();
}

prefix_ senf::scheduler::detail::IdleEventDispatcher::~IdleEventDispatcher()
{
    for (EventList::iterator i (events_.begin()), i_end (events_.end()); i != i_end; ++i)
        FIFORunner::instance().dequeue(&(*i));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IdleEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
