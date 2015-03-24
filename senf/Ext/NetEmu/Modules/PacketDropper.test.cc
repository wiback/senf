// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <tho@berlios.de>

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
