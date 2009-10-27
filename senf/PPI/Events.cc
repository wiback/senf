// $Id$
//
// Copyright (C) 2007
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
    \brief Events non-inline non-template implementation */

#include "Events.hh"
#include "Events.ih"

// Custom includes
#include "Route.hh"
#include "EventManager.hh"

//#include "Events.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
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
    SENF_ASSERT(v_isRegistered() && "Module::registerEvent() call missing");
    if (throttled_ && v)
        return;
    if (v && ! enabled_)
        v_enable();
    else if (! v && enabled_)
        v_disable();
    enabled_ = v;
}

///////////////////////////////cc.e////////////////////////////////////////
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
