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
    \brief NDPMessage unit tests */

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include "NDPOptions.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
SENF_AUTO_TEST_CASE(NDPMessage_create)
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

    senf::NDPNeighborSolicitationMessage::Parser::options_t::container_type roptC(rnadm->options() );
    senf::NDPNeighborSolicitationMessage::Parser::options_t::container_type::const_iterator listIter (roptC.begin());
    BOOST_CHECK( listIter->is<senf::NDPSourceLLAddressTLVParser>() );
    BOOST_CHECK_EQUAL( listIter->type(), 1u );
    BOOST_CHECK_EQUAL( listIter->length(), 1u );
    SENF_CHECK_NO_THROW( listIter->as<senf::NDPSourceLLAddressTLVParser>());
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
