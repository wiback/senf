// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer NETwork research (NET)
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
    \brief AreaRegistry non-inline non-template implementation */

#include "AreaRegistry.hh"
#include "AreaRegistry.ih"

// Custom includes
#include "Target.hh"

//#include "AreaRegistry.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::detail::AreaBase

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

prefix_ void senf::log::detail::AreaBase::write(boost::posix_time::ptime timestamp,
                                                StreamBase const & stream, unsigned level,
                                                std::string msg)
    const
{
    if (stream.index >= routingCache_.size())
        return;
    Routes::iterator i (routingCache_[stream.index].routes.begin());
    Routes::iterator const i_end (routingCache_[stream.index].routes.end());
    for (; i != i_end; ++i)
        i->target->write(timestamp, stream, *this, level, msg);
}

///////////////////////////////cc.e////////////////////////////////////////
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
