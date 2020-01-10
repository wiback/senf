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
    if (binding_ && EventManager::alive())
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
