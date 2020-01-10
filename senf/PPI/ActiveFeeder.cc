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
    \brief ActiveFeeder non-inline non-template implementation */

#include "ActiveFeeder.hh"
//#include "ActiveFeeder.ih"

// Custom includes

//#include "ActiveFeeder.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::ActiveFeeder

prefix_ senf::ppi::module::ActiveFeeder::ActiveFeeder()
    : idle_("Active Feeder")
{
    route(input,idle_);
    route(idle_,output);
    registerEvent(idle_, &ActiveFeeder::request);
}

prefix_ void senf::ppi::module::ActiveFeeder::request()
{
    output(input());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ActiveFeeder.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
