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
    \brief AreaRegistry non-inline non-template implementation */

#include "AreaRegistry.hh"
#include "AreaRegistry.ih"

// Custom includes
#include "Target.hh"

//#include "AreaRegistry.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::detail::AreaBase

prefix_ senf::log::detail::AreaBase::AreaBase()
    : alive_ (true)
{}

prefix_ senf::log::detail::AreaBase::~AreaBase()
{
    alive_ = false;
}

prefix_ void senf::log::detail::AreaBase::updateRoutingCache(Target & target,
                                                             StreamBase const & stream,
                                                             unsigned limit)
    const
{
    if (stream.index >= routingCache_.size())
        routingCache_.resize(stream.index+1);
    unsigned l (limit);
    Routes::iterator i (routingCache_[stream.index].routes.begin());
    Routes::iterator const i_end (routingCache_[stream.index].routes.end());
    for (; i != i_end; ++i) {
        if (i->target == &target) {
            i->limit = limit;
            break;
        }
        if (i->limit < l)
            l = i->limit;
    }
    if (i == i_end)
        routingCache_[stream.index].routes.push_back(RouteEntry(limit, &target));
    else
        for (; i != i_end; ++i)
            if (i->limit < l)
                l = i->limit;
    routingCache_[stream.index].limit = l;
}

prefix_ void senf::log::detail::AreaBase::removeRoutingCache(Target & target,
                                                             StreamBase const & stream)
    const
{
    if (stream.index >= routingCache_.size())
        return;
    unsigned l (DISABLED::value);
    Routes::iterator i (routingCache_[stream.index].routes.begin());
    Routes::iterator const i_end (routingCache_[stream.index].routes.end());
    while (i != i_end) {
        if (i->target == &target)
            i = routingCache_[stream.index].routes.erase(i);
        else {
            if (i->limit < l)
                l = i->limit;
            ++i;
        }
    }
    routingCache_[stream.index].limit = l;
}

prefix_ void senf::log::detail::AreaBase::write(time_type timestamp,
                                                StreamBase const & stream, unsigned level,
                                                std::string const & msg)
    const
{
    if (stream.index >= routingCache_.size())
        return;
    Routes::iterator i (routingCache_[stream.index].routes.begin());
    Routes::iterator const i_end (routingCache_[stream.index].routes.end());
    for (; i != i_end; ++i)
        i->target->write(timestamp, stream, *this, level, msg);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "AreaRegistry.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
