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
    \brief IdleEvent non-inline non-template implementation */

#include "IdleEvent.hh"
//#include "IdleEvent.ih"

// Custom includes

//#include "IdleEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::IdleEvent

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::IdleEvent::v_enable()
{
    timer_.timeout(manager().now());
}

prefix_ void senf::ppi::IdleEvent::v_disable()
{
    timer_.disable();
}

prefix_ void senf::ppi::IdleEvent::cb()
{
    callback();
    if (enabled())
        v_enable();
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
