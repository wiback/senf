// $Id$
//
// Copyright (C) 2010
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
//   Stefan Sauer <ssauer@berlios.de>

/** \file
    \brief NDPMessage unit tests */

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include "NDPOptions.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
SENF_AUTO_UNIT_TEST(NDPMessage_create)
{
    senf::dumpPacketRegistries(std::cout);

    unsigned char data[] = {
            0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, //IPv6
            0x20, 0x01, 0x08, 0x90, 0x06, 0x00, 0xff, 0xff,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01,
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, 0x03, 0x00, 0x02,
            0x87, 0x00, 0xaf, 0x30,                         //ICMPv6
            0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x08, 0x90, //Neighbor Solicitation
            0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x03, 0x00, 0x02,
            0x01, 0x01, 0x00, 0x0d,                         //Source Option
            0x88, 0x70, 0xe4, 0xc0,
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create(data));
    senf::ICMPv6Packet icmp (ip.find<senf::ICMPv6Packet>());

    BOOST_CHECK_EQUAL(icmp->type(), 0x87 );
    BOOST_CHECK( icmp.next() );
    BOOST_CHECK( icmp.next().is<senf::NDPNeighborSolicitationMessage>() );
    senf::NDPNeighborSolicitationMessage rnadm (icmp.next().as<senf::NDPNeighborSolicitationMessage>());
    BOOST_CHECK_EQUAL(rnadm.size(),28);

    senf::NDPNeighborSolicitationMessage::Parser::options_t::container roptC(rnadm->options() );
    senf::NDPNeighborSolicitationMessage::Parser::options_t::container::const_iterator listIter (roptC.begin());
    BOOST_CHECK( listIter->is<senf::NDPSourceLLAddressTLVParser>() );
    BOOST_CHECK_EQUAL( listIter->type(), 1u );
    BOOST_CHECK_EQUAL( listIter->length(), 1u );
    senf::NDPSourceLLAddressTLVParser llopt (listIter->as<senf::NDPSourceLLAddressTLVParser>());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
