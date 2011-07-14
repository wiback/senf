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
    \brief EventHook non-inline non-template implementation */

#include "EventHook.hh"
#include "EventHook.ih"

// Custom includes

//#include "EventHook.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::EventHook

prefix_ void senf::scheduler::EventHook::v_run()
{
    cb_();
}

prefix_ char const * senf::scheduler::EventHook::v_type()
    const
{
    return "ee";
}

prefix_ std::string senf::scheduler::EventHook::v_info()
    const
{
    return "";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::EventHookDispatcher

prefix_ senf::scheduler::detail::EventHookDispatcher::~EventHookDispatcher()
{
    for (EventList::iterator i (events_.begin()); i != events_.end(); ++i)
        FIFORunner::instance().dequeue(&(*i));
}

prefix_ prefix_ void senf::scheduler::detail::EventHookDispatcher::remove(EventHook & event)
{
    EventList::iterator i (EventList::current(event));
    if (i == events_.end())
        return;
    FIFORunner::instance().dequeue(&event);
    events_.erase(i);
}

prefix_ void senf::scheduler::detail::EventHookDispatcher::prepareRun()
{
    for (EventList::iterator i (events_.begin()); i != events_.end(); ++i)
        i->setRunnable();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EventHook.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
