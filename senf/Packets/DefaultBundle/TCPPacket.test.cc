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
    \brief TCPPacket unit tests */

#include "TCPPacket.hh"

// Custom includes
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(tcpPacket_parse)
{
    unsigned char data[] = {
            0x80, 0xA6, 0x00, 0x50, 0x2E, 0x93, 0x31, 0xB8, 0x66, 0xF9, 0xB7,
            0xA1, 0x80, 0x18, 0x00, 0x2E, 0xCF, 0x65, 0x00, 0x00
    };

    senf::TCPPacket p (senf::TCPPacket::create(data));

    BOOST_CHECK_EQUAL( p->source(),               0x80A6u       );
    BOOST_CHECK_EQUAL( p->destination(),          0x0050u       );
    BOOST_CHECK_EQUAL( p->sequencenumber(),       0x2E9331B8u   );
    BOOST_CHECK_EQUAL( p->acknowledgmentnumber(), 0x66F9B7A1u   );
    BOOST_CHECK_EQUAL( p->dataoffset(),           0x8u          );
    BOOST_CHECK_EQUAL( p->urgf(),                 0u            );
    BOOST_CHECK_EQUAL( p->ackf(),                 1u            );
    BOOST_CHECK_EQUAL( p->pshf(),                 1u            );
    BOOST_CHECK_EQUAL( p->rstf(),                 0u            );
    BOOST_CHECK_EQUAL( p->synf(),                 0u            );
    BOOST_CHECK_EQUAL( p->finf(),                 0u            );
    BOOST_CHECK_EQUAL( p->window(),               0x002Eu       );
    BOOST_CHECK_EQUAL( p->checksum(),             0xCF65u       );
    BOOST_CHECK_EQUAL( p->urgentpointer(),        0x0000u       );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_TEST_CASE(tcpPacket_in_ipv4_parse)
{
    unsigned char data[] = {
            //20 Byte IPv4-Header
            0x45, 0x00, 0x00, 0xb2, 0x0f, 0x4d, 0x40, 0x00, 0x0f9, 0x06, 0xa0,
            0xa1, 0xc1, 0x63, 0x90, 0x55, 0x0a, 0x14, 0x75, 0x8a,

            //20 Byte TCP-Header + 12 Byte Options
            0x00, 0x50, 0xc3, 0xbf, 0xe2, 0xf7, 0x49, 0xf3, 0x3e, 0x4f, 0x8a,
            0xbe, 0x80, 0x18, 0x13, 0x24, 0x2e, 0x33, 0x00, 0x00, 0x01, 0x01,
            0x08, 0x0a, 0x0f, 0xf1, 0xa7, 0x6b, 0x00, 0x59, 0xf4, 0x75,

            //TCP-Payload
            0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x20, 0x33, 0x30,
            0x34, 0x20, 0x4e, 0x6f, 0x74, 0x20, 0x4d, 0x6f, 0x64, 0x69, 0x66,
            0x69, 0x65, 0x64, 0x0d, 0x0a, 0x44, 0x61, 0x74, 0x65, 0x3a, 0x20,
            0x54, 0x68, 0x75, 0x2c, 0x20, 0x32, 0x33, 0x20, 0x4a, 0x75, 0x6c,
            0x20, 0x32, 0x30, 0x30, 0x39, 0x20, 0x31, 0x35, 0x3a, 0x31, 0x35,
            0x3a, 0x31, 0x39, 0x20, 0x47, 0x4d, 0x54, 0x0d, 0x0a, 0x53, 0x65,
            0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 0x41, 0x70, 0x61, 0x63, 0x68,
            0x65, 0x2f, 0x32, 0x2e, 0x32, 0x2e, 0x31, 0x31, 0x0d, 0x0a, 0x43,
            0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20,
            0x63, 0x6c, 0x6f, 0x73, 0x65, 0x0d, 0x0a, 0x56, 0x61, 0x72, 0x79,
            0x3a, 0x20, 0x55, 0x73, 0x65, 0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e,
            0x74, 0x0d, 0x0a, 0x0d, 0x0a
    };

    senf::IPv4Packet ip (senf::IPv4Packet::create(data));
    senf::TCPPacket tcp (ip.next<senf::TCPPacket>());

    BOOST_CHECK( tcp->validateChecksum() );
}


SENF_AUTO_TEST_CASE(tcpPacket_in_ipv6_parse)
{
    unsigned char data[] = {
            // IPv6-Header
            0x60, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x06, 0x39, 0x20, 0x01, 0x48,
            0x60, 0xa0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x68, 0x20, 0x01, 0x06, 0x38, 0x04, 0x08, 0x02, 0x01, 0xef,
            0xdc, 0x71, 0x52, 0x17, 0x2c, 0x44, 0x3e,

            // 20 Byte TCP-Header without Options
            0x00, 0x50, 0xac, 0x3e, 0x82, 0x99, 0x60, 0xa6, 0xa4, 0xfa, 0x32,
            0x83, 0x50, 0x18, 0x19, 0x20, 0xd7, 0xcd, 0x00, 0x00,

            // TCP-Payload
            0x1a, 0x01, 0x52, 0x0e, 0x26, 0x6f, 0xac, 0xd9, 0x78, 0x0c, 0x3c,
            0x69, 0x3c, 0xa6, 0x49, 0xc6, 0x85, 0xc2, 0xa9, 0x72, 0x75, 0x9a,
            0xef, 0x63, 0x46, 0xa3, 0xd4, 0x11, 0xda, 0x04, 0x95, 0x55,0x7b,
            0xb5, 0x6c, 0xcb, 0x52, 0xfb, 0xa0, 0x7a, 0x03, 0xd5, 0x98, 0x75,
            0x8b, 0x53, 0xfc, 0x23, 0x21, 0x01, 0xc5, 0x06, 0x66, 0x6c, 0x0d,
            0x06, 0x1d, 0x69, 0x2f, 0xec, 0xb7, 0x7d, 0xe3, 0xd6, 0x71, 0x23,
            0xaa, 0x58, 0x3b, 0x80, 0x58, 0x5b, 0x10, 0xa1, 0xa8, 0x8f, 0x59,
            0x3d, 0x3d, 0x80, 0xd6, 0x2e, 0x0d, 0x21, 0x33, 0x2b, 0x47, 0x5b,
            0xb2, 0xd0, 0xde, 0x19, 0xaf, 0x03, 0x2a, 0x33,0x86, 0x57, 0x8e,
            0x07, 0xf9, 0x6e, 0xde, 0x68, 0xe1, 0xd8, 0x63, 0x92, 0x18, 0x1a,
            0xa9, 0xc0, 0xe5, 0x94, 0xba, 0x1d, 0x97, 0xed, 0x7e, 0x2b, 0xa8,
            0xda, 0x4a, 0x96, 0x0c, 0xe5, 0x78, 0xb9, 0xec, 0x80, 0xf8, 0x5c,
            0x94, 0xf9, 0xd3, 0x49, 0x79, 0x4a, 0x4a, 0x98, 0xb1, 0x13, 0x73,
            0x50, 0x6c
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create(data));
    senf::TCPPacket tcp (ip.next<senf::TCPPacket>());

    BOOST_CHECK( tcp->validateChecksum() );
}

SENF_AUTO_TEST_CASE(tcpPacket_in_ipv6_create)
{
    unsigned char data[] = {
            // IPv6-Header
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x06, 0x40,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,

            // TCP-Header + TCP-Payload
            0x5b, 0xa0, 0x30, 0x39, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00,
            0x18, 0x80, 0x00, 0x00, 0x54, 0x45, 0x53, 0x54,
            0x2d, 0x57, 0x52, 0x49, 0x54, 0x45
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create());
    ip->source() = senf::INet6Address::Loopback;
    ip->destination() = senf::INet6Address::Loopback;
    ip->hopLimit() = 64;

    senf::TCPPacket tcp (senf::TCPPacket::createAfter(ip));
    tcp->source() = 23456;
    tcp->destination() = 12345;

    senf::DataPacket::createAfter(tcp, std::string("TEST-WRITE"));

    // validates, since the checksum is 0 and thus ignored !
    BOOST_CHECK( tcp->validateChecksum() );

    ip.finalizeAll();

    BOOST_CHECK_EQUAL_COLLECTIONS(
            data, data+sizeof(data), ip.data().begin(), ip.data().end() );
    BOOST_CHECK( tcp->validateChecksum() );
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
