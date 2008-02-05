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

//#include "UDPPacket.test.hh"
//#include "UDPPacket.test.ih"

// Custom includes
#include "UDPPacket.hh"
#include "IPv4Packet.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(udpPacket_packet)
{

    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08
                           };

    senf::UDPPacket p (senf::UDPPacket::create(data));

    BOOST_CHECK_EQUAL( p->source(),            0x0102       );
    BOOST_CHECK_EQUAL( p->destination(),       0x0304       );
    BOOST_CHECK_EQUAL( p->length(),            0x0506       );
    BOOST_CHECK_EQUAL( p->checksum(),          0x0708       );
}

BOOST_AUTO_UNIT_TEST(udpPacket_create)
{
    unsigned char data[] = { 0x45, 0x00, 0x00, 0x26, 0x00, 0x00, 0x40, 0x00,
                             0x40, 0x11, 0x3c, 0xc5, 0x7f, 0x00, 0x00, 0x01,
                             0x7f, 0x00, 0x00, 0x01, 0x5b, 0xa0, 0x30, 0x39,
                             0x00, 0x12, 0xfa, 0x6e, 0x54, 0x45, 0x53, 0x54,
                             0x2d, 0x57, 0x52, 0x49, 0x54, 0x45 };

    senf::IPv4Packet ip (senf::IPv4Packet::create());
    ip->source() = senf::INet4Address::Loopback;
    ip->destination() = senf::INet4Address::Loopback;
    ip->df() = true;
    ip->ttl() = 64;
    
    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));
    udp->source() = 23456;
    udp->destination() = 12345;
    
    senf::DataPacket::createAfter(udp,std::string("TEST-WRITE"));

    // validates, since the checksum is 0 and thus ignored !
    BOOST_CHECK( udp->validateChecksum() );

    ip.finalize();
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
                                   ip.data().begin(), ip.data().end() );
    BOOST_CHECK( udp->validateChecksum() );
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
