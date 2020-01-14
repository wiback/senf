//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ThrottleBarrier unit tests */

#include "ThrottleBarrier.hh"

// Custom includes
#include <senf/Packets/Packets.hh>
#include "DebugModules.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(throttleBarrier)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::ThrottleBarrier barrier;
    senf::ppi::module::debug::PassiveSink sink;

    senf::ppi::connect(source,barrier);
    senf::ppi::connect(barrier,sink);

    senf::Packet p (senf::DataPacket::create());

    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    sink.throttle();
    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
