// $Id$
//
// Copyright (C) 2009
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
    \brief IdleEvent non-inline non-template implementation */

#include "IdleEvent.hh"
#include "IdleEvent.ih"

// Custom includes

//#include "IdleEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
