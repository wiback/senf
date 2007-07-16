// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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

/** \file
    \brief IpV6Extensions.test unit tests */

//#include "IpV6Extensions.test.hh"
//#include "IpV6Extensions.test.ih"

// Custom includes
#include "IpV6Extensions.hh"
#include "IpV6Packet.hh"
#include "UDPPacket.hh"
#include "Packets/DataPacket.hh"
#include "Socket/INetAddressing.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ipv6Extension_Fragment_packet)
{
    // Just for the fun of it, we test a nice chain: A fragment of a fragmented UDP packet

    unsigned char data[] = {
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

    senf::IpV6Packet p (senf::IpV6Packet::create(data));

    BOOST_CHECK_EQUAL( p->version(), 6u );
    BOOST_CHECK_EQUAL( p->length(), 20u );
    BOOST_CHECK_EQUAL( p->nextHeader(), 44u );
    BOOST_CHECK_EQUAL( senf::INet6Address(p->source()), "2001::1" );
    BOOST_CHECK_EQUAL( senf::INet6Address(p->destination()), "2001::2" );
    BOOST_CHECK( p.next().is<senf::IpV6Extension_Fragment>() );

    senf::IpV6Extension_Fragment f (p.next().as<senf::IpV6Extension_Fragment>());

    BOOST_CHECK_EQUAL( f->nextHeader(), 17u );
    BOOST_CHECK_EQUAL( f->fragmentOffset(), 160u );
    BOOST_CHECK_EQUAL( f->id(), 0x01020304u );
    BOOST_CHECK( f.next().is<senf::UDPPacket>() );

    senf::UDPPacket u (f.next().as<senf::UDPPacket>());

    BOOST_CHECK_EQUAL( u->source(), 0x1000u );
    BOOST_CHECK_EQUAL( u->destination(), 0x2000u );
    BOOST_CHECK_EQUAL( u->length(), 12u );
    BOOST_CHECK( u.next().is<senf::DataPacket>() );

    senf::DataPacket d (u.next().as<senf::DataPacket>());
    senf::Packet::iterator i (u.next().data().begin());
    BOOST_CHECK_EQUAL( d.size(), 4u );
    BOOST_CHECK_EQUAL( d.data()[0], 0x11 );
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
