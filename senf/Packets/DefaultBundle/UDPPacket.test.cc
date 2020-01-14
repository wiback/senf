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
    \brief UDPPacket unit tests */

#include "UDPPacket.hh"

// Custom includes
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(udpPacket_parse)
{

    unsigned char data[] = {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    senf::UDPPacket p (senf::UDPPacket::create(data));

    BOOST_CHECK_EQUAL( p->source(),            0x0102       );
    BOOST_CHECK_EQUAL( p->destination(),       0x0304       );
    BOOST_CHECK_EQUAL( p->length(),            0x0506       );
    BOOST_CHECK_EQUAL( p->checksum(),          0x0708       );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_TEST_CASE(udpPacket_in_ipv4_create)
{
    unsigned char data[] = {
            0x45, 0x00, 0x00, 0x26, 0x00, 0x00, 0x40, 0x00,
            0x40, 0x11, 0x3c, 0xc5, 0x7f, 0x00, 0x00, 0x01,
            0x7f, 0x00, 0x00, 0x01, 0x5b, 0xa0, 0x30, 0x39,
            0x00, 0x12, 0xfa, 0x6e, 0x54, 0x45, 0x53, 0x54,
            0x2d, 0x57, 0x52, 0x49, 0x54, 0x45
    };

    senf::IPv4Packet ip (senf::IPv4Packet::create());
    ip->source() = senf::INet4Address::Loopback;
    ip->destination() = senf::INet4Address::Loopback;
    ip->df() = true;
    ip->ttl() = 64;

    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));
    udp->source() = 23456;
    udp->destination() = 12345;

    senf::DataPacket::createAfter(udp, std::string("TEST-WRITE"));

    // validates, since the checksum is 0 and thus ignored !
    BOOST_CHECK( udp->validateChecksum() );

    ip.finalizeAll();
    BOOST_CHECK_EQUAL_COLLECTIONS(
            data, data+sizeof(data), ip.data().begin(), ip.data().end() );
    BOOST_CHECK( udp->validateChecksum() );
}

SENF_AUTO_TEST_CASE(udpPacket_in_ipv6_parse)
{
    // captured udp packet generated with mgen send over ipv6
    unsigned char data[] = {
            // IPv6 Packet
            0x60, 0x00, 0x00, 0x00, 0x00, 0x32, 0x11, 0x40,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            // UDP Packet
            0x13, 0x89, 0x13, 0x88, 0x00, 0x32, 0x11, 0x23,
            // mgen payload
            0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00, 0x0b, 0x49, 0xb5, 0x0a, 0x90,
            0x00, 0x09, 0x5b, 0x37, 0x13, 0x88, 0x02, 0x10,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create(data));
    BOOST_CHECK_EQUAL( ip->length(),     50u  );
    BOOST_CHECK_EQUAL( ip->nextHeader(), 0x11 );
    BOOST_CHECK_EQUAL( ip->hopLimit(),   64u  );
    BOOST_CHECK_EQUAL( ip->source().value(),      senf::INet6Address::Loopback );
    BOOST_CHECK_EQUAL( ip->destination().value(), senf::INet6Address::Loopback );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( ip.dump( oss));

    BOOST_REQUIRE( ip.next().is<senf::UDPPacket>() );
    senf::UDPPacket udp (ip.next().as<senf::UDPPacket>());

    BOOST_CHECK_EQUAL( udp->source(),      5001u  );
    BOOST_CHECK_EQUAL( udp->destination(), 5000u  );
    BOOST_CHECK_EQUAL( udp->length(),      50u    );
    BOOST_CHECK_EQUAL( udp->checksum(),    0x1123 );

    BOOST_CHECK( udp->validateChecksum() );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
