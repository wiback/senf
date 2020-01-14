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
    \brief PacketDropper unit tests */

#include "PacketDropper.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/floating_point_comparison.hpp>

// Custom includes
#include <senf/PPI/DebugModules.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
#define NUMBER_OF_TESTPACKETS 5000

SENF_AUTO_TEST_CASE(packetDropper)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    senf::emu::PacketDropper dropper (1); // drop all packets

    senf::ppi::connect( source, dropper);
    senf::ppi::connect( dropper, sink);

    senf::Packet p (senf::DataPacket::create());
    for (long i=0; i<NUMBER_OF_TESTPACKETS; ++i)
        source.submit( p);
    BOOST_CHECK_EQUAL( sink.size(), 0);
    BOOST_CHECK_EQUAL( dropper.lossRate(), 1);

    sink.clear();

    dropper.lossRate(0); // drop no packets
    for (long i=0; i<NUMBER_OF_TESTPACKETS; ++i)
        source.submit( p);
    BOOST_CHECK_EQUAL( sink.size(), NUMBER_OF_TESTPACKETS);
    BOOST_CHECK_EQUAL( dropper.lossRate(), 0);

    sink.clear();

    dropper.lossRate(0.5); // drop a packet with a probability of 50%
    for (long i=0; i<NUMBER_OF_TESTPACKETS; ++i)
        source.submit( p);
    // nearly 50% of NUMBER_OF_TESTPACKETS must have arrived the sink
    // we accept a variation of 10%
    BOOST_CHECK_CLOSE( double(sink.size())/NUMBER_OF_TESTPACKETS, .5, 10 );
    BOOST_CHECK_EQUAL( dropper.lossRate(), .5);

    sink.clear();

    dropper.lossRate(0.01); // drop a packet with a probability of 1%
    for (long i=0; i<NUMBER_OF_TESTPACKETS; ++i)
        source.submit( p);
    // nearly 99% of NUMBER_OF_TESTPACKETS must have arrived the sink
    // we accept a variation of 3%
    BOOST_CHECK_CLOSE( double(sink.size())/NUMBER_OF_TESTPACKETS, .99, 3 );
    BOOST_CHECK_EQUAL( dropper.lossRate(), .01);
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
