// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
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

// Unit tests

//#include "IPv4Packet.test.hh"
//#include "IPv4Packet.test.ih"

// Custom includes
#include "IPv4Packet.hh"
#include "UDPPacket.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(ipV4Packet_packet)
{

    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C,
                             0x11, 0x12, 0x13, 0x14,
                             0x15, 0x16, 0x17, 0x18
                           };

    senf::IPv4Packet p (senf::IPv4Packet::create(data));

    BOOST_CHECK_EQUAL( p->version(),     0x00u       );
    BOOST_CHECK_EQUAL( p->ihl(),         0x01u       );
    BOOST_CHECK_EQUAL( p->tos(),         0x02u       );
    BOOST_CHECK_EQUAL( p->length(),      0x0304u     );
    BOOST_CHECK_EQUAL( p->identifier(),  0x0506u     );
    BOOST_CHECK_EQUAL( p->reserved(),    0           );
    BOOST_CHECK_EQUAL( p->df(),          0           );
    BOOST_CHECK_EQUAL( p->mf(),          0           );
    BOOST_CHECK_EQUAL( p->frag(),        0x0708u     );
    BOOST_CHECK_EQUAL( p->ttl(),         0x09u       );
    BOOST_CHECK_EQUAL( p->protocol(),    0x0Au       );
    BOOST_CHECK_EQUAL( p->checksum(),    0x0B0Cu     );
    BOOST_CHECK_EQUAL( p->source().value(), senf::INet4Address(0x11121314u) );
    BOOST_CHECK_EQUAL( p->destination().value(), senf::INet4Address(0x15161718u) );
}

BOOST_AUTO_UNIT_TEST(ipV4Packet_create)
{
    senf::IPv4Packet ip (senf::IPv4Packet::create());

    BOOST_CHECK_EQUAL( ip->version(), 4u );
    BOOST_CHECK_EQUAL( ip->ihl(), 5u );
    BOOST_CHECK_EQUAL( ip.size(), 20u );

    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));

    BOOST_CHECK( ! ip->validateChecksum() );

    ip.finalize();
    BOOST_CHECK_EQUAL( ip->length(), 28u );
    BOOST_CHECK_EQUAL( ip->protocol(), 17u );
    BOOST_CHECK_EQUAL( ip->checksum(), 0xbad2 );

    // Check, that the checksum field is correctly skipped
    ip.finalize();
    BOOST_CHECK_EQUAL( ip->checksum(), 0xbad2 );

    BOOST_CHECK( ip->validateChecksum() );
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
