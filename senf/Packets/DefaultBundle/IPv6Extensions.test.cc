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
#include "ICMPv6TypePacket.hh"
#include "EthernetPacket.hh"
#include <senf/Utils/hexdump.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ipv6Extensions_fragment)
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
    BOOST_CHECK_EQUAL( pFragment_packet->source().value(), 
            senf::INet6Address::from_string("2001::1") );
    BOOST_CHECK_EQUAL( pFragment_packet->destination().value(), 
            senf::INet6Address::from_string("2001::2") );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( pFragment_packet.dump( oss));

    BOOST_CHECK( pFragment_packet.next().is<senf::IPv6FragmentPacket>() );

    senf::IPv6FragmentPacket fFragment_packet (pFragment_packet.next().as<senf::IPv6FragmentPacket>());

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
}


BOOST_AUTO_UNIT_TEST(ipv6Extensions_routing)
{
    unsigned char Routing_packetData[] = {
        // IP header
        0x60, 0x30, 0x00, 0x00, 
        0x00, 0x10,             // payload Length
        0x2b,                   // next Header   (43 = Routing Header)
        0xff,                   // Hop Limit
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x02, 0x01, 0x02, 0xff, 0xfe, 0x00, 0x02, 0x00, // Src Addr
        0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66,
        0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, // Dest. Addr
        // Routing Header
        0x3a,                   // nextHeader (58)
        0x00,                   // length ( 0 )
        0x00,                   // routing type ( 0 )
        0x00,                   // Left Segments ( 0 )
        0x00, 0x00, 0x00, 0x00, // reserved
        // ICMPv6
        0x01,                   // type: 1 (unreachable )
        0x00,                   // code: 0 (route unreachable)
        0xa3, 0xd3,             // checksum (incorrect in wireshark capture file, should be 0xa3c4)
        0x00, 0x00, 0x00, 0x00
    };
    
    senf::IPv6Packet pRouting_packet (senf::IPv6Packet::create(Routing_packetData));

    BOOST_CHECK_EQUAL( pRouting_packet->version(), 6u );
    BOOST_CHECK_EQUAL( pRouting_packet->length(), 16u );
    BOOST_CHECK_EQUAL( pRouting_packet->nextHeader(), 43u );
    BOOST_CHECK_EQUAL( pRouting_packet->source().value(), 
            senf::INet6Address::from_string("fe80::201:2ff:fe00:200") );
    BOOST_CHECK_EQUAL( pRouting_packet->destination().value(), 
            senf::INet6Address::from_string("3555:5555:6666:6666:7777:7777:8888:8888"));
    
    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( pRouting_packet.dump( oss));

    BOOST_REQUIRE( pRouting_packet.next().is<senf::IPv6RoutingPacket>() );

    senf::IPv6RoutingPacket pRouting_extension (pRouting_packet.next().as<senf::IPv6RoutingPacket>());

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


BOOST_AUTO_UNIT_TEST(ipv6Extensions_hopByHop_parse)
{
    unsigned char HopByHop_packetData[] = {
        0x60, 0x00, 0x00, 0x00, // IP version, class, flow label
        0x00, 0x24,             // payload length
        0x00,                   // next header: IPv6 hop-by-hop option (0)
        0x01,                   // hop limit (1)
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x02, 0x19, 0xb9, 0xff, 0xfe, 0xeb, 0xb2, 0x26, // IPv6 Source address
        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, // IPv6 Destination address ff02::16
        // HopByHop option
        0x3a, // next Header (ICMPv6)
        0x00, // Length (0 = 8Bytes)
        // option Header
        0x05, // option type
        0x02, // option Length (= 2 byte)
        0x00, 0x00, //data (zero data here ...)
        0x02, //option type (2, set for testing purposes only)
        0x00,  //option Type length (=0, no data field needed here)
        // ICMPv6
        0x8f, // type 143
        0x00, // code 0, should always be 0
        0x50, 0xcc, // checksum
    };

    senf::IPv6Packet pHop_packet (senf::IPv6Packet::create(HopByHop_packetData));
    BOOST_CHECK_EQUAL( pHop_packet->version(), 6u );
    BOOST_CHECK_EQUAL( pHop_packet->length(), 36u );
    BOOST_CHECK_EQUAL( pHop_packet->nextHeader(), 0u );
    BOOST_CHECK_EQUAL( pHop_packet->source().value(), 
            senf::INet6Address::from_string("fe80::219:b9ff:feeb:b226") );
    BOOST_CHECK_EQUAL( pHop_packet->destination().value(), senf::INet6Address::from_string("ff02::16") );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( pHop_packet.dump( oss ));

    BOOST_REQUIRE( pHop_packet.next().is<senf::IPv6HopByHopOptionsPacket>() );
    senf::IPv6HopByHopOptionsPacket pHop_extension (pHop_packet.next().as<senf::IPv6HopByHopOptionsPacket>());
    BOOST_CHECK_EQUAL( pHop_extension->nextHeader(), 58u );
    BOOST_CHECK_EQUAL( pHop_extension->headerLength(), 0x00 );

    senf::IPv6HopByHopOptionsPacket::Parser::options_t::container optC(pHop_extension->options() );
    senf::IPv6HopByHopOptionsPacket::Parser::options_t::container::iterator listIter (optC.begin());

    BOOST_CHECK_EQUAL( listIter->altAction(), 0u);
    BOOST_CHECK_EQUAL( listIter->changeFlag(), 0u);
    BOOST_CHECK_EQUAL( listIter->optionType(), 5u);
    BOOST_CHECK_EQUAL( listIter->optionLength(), 2u);
    ++listIter;
    BOOST_CHECK_EQUAL( listIter->altAction(), 0u);
    BOOST_CHECK_EQUAL( listIter->changeFlag(), 0u);
    BOOST_CHECK_EQUAL( listIter->optionType(), 2u);
    BOOST_CHECK_EQUAL( listIter->optionLength(), 0);

    BOOST_REQUIRE( pHop_extension.next().is<senf::ICMPv6Packet>() );
    senf::ICMPv6Packet pICMPv6  (pHop_extension.next().as<senf::ICMPv6Packet>());
    BOOST_CHECK_EQUAL( pICMPv6->type(), 0x8f);
    BOOST_CHECK_EQUAL( pICMPv6->code(), 0u);
    BOOST_CHECK_EQUAL( pICMPv6->checksum(), 0x50cc);
}

BOOST_AUTO_UNIT_TEST(ipv6Extensions_hopByHop_create)                                                                                                      
{   
    try{
    std::ostringstream oss (std::ostringstream::out);                                                                                                   
    unsigned char HopByHop_packetData[] = {
        //Ethernet
        0x33 ,0x33 ,0x00 ,0x00 ,0x00 ,0x16 ,    //destination MAC
        0x00 ,0x19 ,0xb9 ,0xeb ,0xb2 ,0x26 ,    //source MAC
        0x86 ,0xdd,                             //type (IPv6)
        //IPv6
        0x60, 0x00, 0x00, 0x00, //IP version, class, flow label
        0x00, 0x24,           //payload length
        0x00,                 //next header: IPv6 hop-by-hop option (0)
        0x01,                 //hop limit (1)
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //IPv6 Source address (fe80::219:b9ff:feeb:b226)
        0x02, 0x19, 0xb9, 0xff, 0xfe, 0xeb, 0xb2, 0x26,

        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //IPv6 Destination address ff02::16
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
        //HopByHop option
        0x3a,   //next Header (ICMPv6)
        0x00,   //Length (0 = 8Bytes)

        //option Header
        0x05, //option type
        0x02, //option Length (= 2 byte)
        0x00, 0x00, //data (zero data here ...)
        0x01, 0x00, //padding
        //ICMPv6
        0x8f, //type 143
        0x00, //code 0, should always be 0
        0x50 ,0xcc , //checksum

        //MLDv2
        0x00 ,0x00 , //reserved, zero by default
        0x00 ,0x01 , //nr of McAddressRecords
        0x04 ,  //recordType
        0x00 ,  //auxiliarydatalength
        0x00 ,0x00 ,    // number of sources
        0xff ,0x02 ,0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00 ,    //ipv6McAddress ff02::1:ffeb:b226
        0x00 ,0x00 ,0x00 ,0x01 ,0xff ,0xeb ,0xb2 ,0x26
    };
    senf::EthernetPacket eth (senf::EthernetPacket::create() );
    eth->destination() = senf::MACAddress::from_string( "33:33:00:00:00:16" );
    eth->source() = senf::MACAddress::from_string( "00:19:b9:eb:b2:26" );
    eth->type_length() = 0x86dd;

    senf::IPv6Packet ip (senf::IPv6Packet::createAfter(eth));
    ip->version() = 6u;
    ip->length() = 12u;
    ip->nextHeader() = 0u;
    ip->hopLimit() = 1u;
    ip->source() = senf::INet6Address::from_string("fe80::219:b9ff:feeb:b226");
    ip->destination() = senf::INet6Address::from_string("ff02::16");

    senf::IPv6HopByHopOptionsPacket pext (senf::IPv6HopByHopOptionsPacket::createAfter(ip) );
    pext->nextHeader() = 58u;
    pext->headerLength() = 0u;
    {
        senf::IPv6HopByHopOptionsPacket::Parser::options_t::container optC(pext->options() );
        {
            senf::IPv6GenericOptionTLVParser opt ( optC.push_back_space().init<senf::IPv6GenericOptionTLVParser>());
            opt.altAction() = 0u;
            opt.changeFlag() = 0u;
            opt.optionType() = 5u;
            unsigned char val[] = {0x00, 0x00};
            opt.setPayload(val);
        }
    }

    senf::ICMPv6Packet icmp (senf::ICMPv6Packet::createAfter (pext));
    icmp->type() = 0x8f;
    icmp->code() = 0u;
    icmp->checksum() = 0u;
    senf::MLDv2ListenerReport mld ( senf::MLDv2ListenerReport::createAfter(icmp) );
    {
        senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container c (mld->mcastAddrRecords() );
        c.push_back_space();
        c.back().recordType() = 4u;
        senf::INet6Address addr( senf::INet6Address::from_string("ff02::1:ffeb:b226") );
        c.back().mcAddress() = addr;
    }
    eth.finalizeAll();
    SENF_CHECK_NO_THROW( ip.dump(oss) );
    SENF_CHECK_EQUAL_COLLECTIONS(
            HopByHop_packetData, HopByHop_packetData+sizeof(HopByHop_packetData),
            eth.data().begin(), eth.data().end() );
    eth.dump(std::cout);
    senf::hexdump(eth.data().begin(), eth.data().end(), std::cout);
    std::cout << "\n\n" << std::endl;
    senf::hexdump(boost::begin(HopByHop_packetData), boost::end(HopByHop_packetData), std::cout);
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl; throw;
    }
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
