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
    \brief DebugModules non-inline non-template implementation */

#include "DebugModules.hh"
//#include "DebugModules.ih"

// Custom includes

//#include "DebugModules.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::debug::PassiveSource

prefix_ void senf::ppi::module::debug::PassiveSource::request()
{
    SENF_ASSERT( ! packets_.empty(),
                 "senf::ppi::module::debug::PassiveSource::request(): "
                 "Requesting packet from empty source." );
    output(packets_.front());
    packets_.pop_front();
    if (packets_.empty())
        output.throttle();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::debug::PassiveSink

prefix_ void senf::ppi::module::debug::PassiveSink::request()
{
    packets_.push_back(input());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "DebugModules.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
