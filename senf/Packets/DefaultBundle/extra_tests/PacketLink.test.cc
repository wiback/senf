// $Id$
//
// Copyright (C) 2008
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

/** \file
    \brief PacketLink unit tests */

//#include "PacketLink.test.hh"
//#include "PacketLink.test.ih"

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(packetLink)
{
    // This test shall test, whether referencing a packet (in this case IPv6Packet) as template
    // argument correctly includes the corresponding object file when linking

    boost::uint8_t data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,             // source mac
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x02,             // target mac
                              0x86, 0xdd,                                     // ethertype

                              0x60, 0x00, 0x00, 0x00,                         // IP version, class,
                                                                              //          flow label
                              0x00, 0x00,                                     // payload length
                              0x3B,                                           // no next header
                              0x10,                                           // hop limit
                              0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // source ip
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                              0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // target ip
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

    senf::EthernetPacket p (senf::EthernetPacket::create(data));

    BOOST_REQUIRE( p.next() );
    BOOST_CHECK( p.next().is<senf::IPv6Packet>() );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
