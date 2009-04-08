// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//     Philipp Batroff <philipp.batroff@fokus.fraunhofer.de>
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief IPv6Extensions unit tests */

//#include "IPv6Extensions.test.hh"
//#include "IPv6Extensions.test.ih"

// Custom includes
#include "IPv6Extensions.hh"
#include "IPv6Packet.hh"
#include "UDPPacket.hh"
#include "ICMPv6Packet.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ipv6Extensions)
{
    // Just for the fun of it, we test a nice chain: A fragment of a fragmented UDP packet

    unsigned char Fragment_packetData[] = {
        // IP header
        0x60, 0x00, 0x00, 0x00,         // IP Version, class, flow label
        0, 20,                          // payload length
        44,                             // next header (IPv6 Fragment)
        32,                             // hop limit
        0x20, 0x01, 0, 0, 0, 0, 0, 0,   // source ip = 2001::1
           0, 0, 0, 0, 0, 0, 0, 0x01,
        0x20, 0x01, 0, 0, 0, 0, 0, 0,   // destination ip = 2001::2
           0, 0, 0, 0, 0, 0, 0, 0x02,
        // IPv6 Fragment header
        17,                             // next header (UDP)
        0,                              // reserved
        0x05, 0x00,                     // fragment offset, last fragment
        0x01, 0x02, 0x03, 0x04,         // id
        // UDP header
        0x10, 0x00,                     // source port
        0x20, 0x00,                     // destination port
        0, 12,                          // length
        0x00, 0x00,                     // CRC (no, I won't calculate this one ...)
        // Payload data
        0x11, 0x12, 0x13, 0x14
    };

    senf::IPv6Packet pFragment_packet (senf::IPv6Packet::create(Fragment_packetData));

    BOOST_CHECK_EQUAL( pFragment_packet->version(), 6u );
    BOOST_CHECK_EQUAL( pFragment_packet->length(), 20u );
    BOOST_CHECK_EQUAL( pFragment_packet->nextHeader(), 44u );
    BOOST_CHECK_EQUAL( pFragment_packet->source().value(), senf::INet6Address::from_string("2001::1") );
    BOOST_CHECK_EQUAL( pFragment_packet->destination().value(), senf::INet6Address::from_string("2001::2") );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( pFragment_packet.dump( oss));

    BOOST_CHECK( pFragment_packet.next().is<senf::IPv6Extension_Fragment>() );

    senf::IPv6Extension_Fragment fFragment_packet (pFragment_packet.next().as<senf::IPv6Extension_Fragment>());

    BOOST_CHECK_EQUAL( fFragment_packet->nextHeader(), 17u );
    BOOST_CHECK_EQUAL( fFragment_packet->fragmentOffset(), 160u );
    BOOST_CHECK_EQUAL( fFragment_packet->id(), 0x01020304u );
    BOOST_CHECK( fFragment_packet.next().is<senf::UDPPacket>() );

    senf::UDPPacket uFragment_packet (fFragment_packet.next().as<senf::UDPPacket>());

    BOOST_CHECK_EQUAL( uFragment_packet->source(), 0x1000u );
    BOOST_CHECK_EQUAL( uFragment_packet->destination(), 0x2000u );
    BOOST_CHECK_EQUAL( uFragment_packet->length(), 12u );
    BOOST_CHECK( uFragment_packet.next().is<senf::DataPacket>() );

    senf::DataPacket dFragment_packet (uFragment_packet.next().as<senf::DataPacket>());
    senf::PacketData::iterator i (uFragment_packet.next().data().begin());
    BOOST_CHECK_EQUAL( dFragment_packet.size(), 4u );
    BOOST_CHECK_EQUAL( dFragment_packet.data()[0], 0x11 );

    //==============================================================================================
    
    unsigned char Routing_packetData[] = {
        // IP header
        0x60, 0x30, 0x00, 0x00, 
        0x00, 0x10,             //payload Length
        0x2b,                   //next Header   (43 = Routing Header)
        0xff,                   //Hop Limit
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x02, 0xff, 0xfe, 0x00, 0x02, 0x00, //Src Addr
        0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, //Dest. Addr
        //Routing Header
        0x3a,                   //nextHeader (58)
        0x00,                   //Length ( 0 )
        0x00,                   //routing type ( 0 )
        0x00,                   //Left Segments ( 0 )
        0x00, 0x00, 0x00, 0x00, //reserved
        //ICMPv6
        0x01,                   //type: 1 (unreachable )
        0x00,                   //code: 0 (route unreachable)
        0xa3, 0xd3,             //checksum (incorrect in wireshark capture file, should be 0xa3c4)
        0x00, 0x00, 0x00, 0x00
    };
    
    senf::IPv6Packet pRouting_packet (senf::IPv6Packet::create(Routing_packetData));

    BOOST_CHECK_EQUAL( pRouting_packet->version(), 6u );
    BOOST_CHECK_EQUAL( pRouting_packet->length(), 16u );
    BOOST_CHECK_EQUAL( pRouting_packet->nextHeader(), 43u );
    BOOST_CHECK_EQUAL( pRouting_packet->source().value(), senf::INet6Address::from_string("fe80::201:2ff:fe00:200") );
    BOOST_CHECK_EQUAL( pRouting_packet->destination().value(), senf::INet6Address::from_string("3555:5555:6666:6666:7777:7777:8888:8888"));
    SENF_CHECK_NO_THROW( pRouting_packet.dump( oss));

    BOOST_REQUIRE( pRouting_packet.next().is<senf::IPv6Extension_Routing>() );

    senf::IPv6Extension_Routing pRouting_extension (pRouting_packet.next().as<senf::IPv6Extension_Routing>());

    BOOST_CHECK_EQUAL( pRouting_extension->nextHeader(), 58u );
    BOOST_CHECK_EQUAL( pRouting_extension->headerLength(), 0x00 );
    BOOST_CHECK_EQUAL( pRouting_extension->routingType(), 0x00 );
    BOOST_CHECK_EQUAL( pRouting_extension->segmentsLeft(), 0x00);
    
    BOOST_CHECK_EQUAL( pRouting_extension->reserved(), 0u);

    BOOST_REQUIRE( pRouting_extension.next().is<senf::ICMPv6Packet>() );
    senf::ICMPv6Packet pICMPv6 (pRouting_extension.next().as<senf::ICMPv6Packet>());
    BOOST_CHECK_EQUAL( pICMPv6->type(), 1u);
    BOOST_CHECK_EQUAL( pICMPv6->code(), 0u);
    BOOST_CHECK_EQUAL( pICMPv6->checksum(), 0xa3d3);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
