// $Id$
//
// Copyright (C) 2007
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
    \brief Events non-inline non-template implementation */

#include "Events.hh"
#include "Events.ih"

// Custom includes
#include "Route.hh"
#include "EventManager.hh"

//#include "Events.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::EventDescriptor

prefix_ senf::ppi::EventDescriptor::~EventDescriptor()
{
    if (binding_)
        binding_->manager().destroyEvent(*this);
}

prefix_ void senf::ppi::EventDescriptor::notifyUnthrottle()
{
    Routes::const_iterator i (routes_.begin());
    Routes::const_iterator const i_end (routes_.end());
    for (; i != i_end; ++i)
        if ((*i)->throttled())
            break;
    if (i != i_end)
        return;
    throttled_ = false;
    enabled(true);
}

prefix_ void senf::ppi::EventDescriptor::enabled(bool v)
{
    SENF_ASSERT(v_isRegistered(), "Module::registerEvent() call missing");
    if (throttled_ && v)
        return;
    if (v && ! enabled_)
        v_enable();
    else if (! v && enabled_)
        v_disable();
    enabled_ = v;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Events.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
