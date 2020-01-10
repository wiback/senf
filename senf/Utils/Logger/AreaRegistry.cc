//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
