// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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
