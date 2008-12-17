// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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

// Definition of non-inline non-template functions

// Custom includes

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

BOOST_AUTO_UNIT_TEST(ICMPv6Packet_packet)
{
    unsigned char data[] = { 
            0x8f, 0x00, 0x8d, 0x54, 0x00, 0x00, 0x00, 0x01,
            0x04, 0x00, 0x00, 0x00, 0xff, 0x15, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x16
    };

    senf::ICMPv6Packet p ( senf::ICMPv6Packet::create(data) );
    
    BOOST_CHECK_EQUAL( p->type(),     0x8f   );
    BOOST_CHECK_EQUAL( p->code(),     0x00   );
    BOOST_CHECK_EQUAL( p->checksum(), 0x8d54 );
    BOOST_CHECK( p.next() );
    BOOST_CHECK( p.next().is<senf::MLDv2ListenerReport>() );
    BOOST_CHECK_EQUAL( p.next().size(), 24u );

}
