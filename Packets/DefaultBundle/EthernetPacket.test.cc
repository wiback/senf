// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

//#include "EthernetPacket.test.hh"
//#include "EthernetPacket.test.ih"

// Custom includes
#include "EthernetPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(ethernetPacket_parser)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
                             0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
                             0x10, 0x11 };                        // EtherType
    typedef unsigned char * iterator;
    Parse_Ethernet<iterator> p(data);

    BOOST_CHECK_EQUAL( p.destination()[2], 0x03 );
    BOOST_CHECK_EQUAL( p.source()[3], 0x0A );
    BOOST_CHECK_EQUAL( p.type(), 0x1011 );
}

BOOST_AUTO_UNIT_TEST(ethernetPacket_packet)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
                             0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
                             0x10, 0x11 };                        // EtherType
    EthernetPacket::ptr p (Packet::create<EthernetPacket>(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->destination()[3], 0x04 );
    BOOST_CHECK_EQUAL( p->source()[0], 0x07 );
    BOOST_CHECK_EQUAL( p->type(), 0x1011 );

    BOOST_CHECK_THROW( Packet::create<EthernetPacket>(data, data+sizeof(data)-1),
                       TruncatedPacketException );
}

BOOST_AUTO_UNIT_TEST(ethernetPacket_chain)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
                             0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
                             0x81, 0x00,                          // EtherType: VLan
                             0x92, 0x34,                          // VLAN prio, cfi, id
                             0xab, 0xcd,                          // EtherType
                             0xf0, 0xf1, 0xf2, 0xf3, 0xf4 };      // Payload

    EthernetPacket::ptr p (Packet::create<EthernetPacket>(data, data+sizeof(data)));

    BOOST_CHECK( p->next()->is<EthVLanPacket>() );
    EthVLanPacket::ptr v (p->next()->as<EthVLanPacket>());
    BOOST_CHECK_EQUAL( v->priority(), 4u );
    BOOST_CHECK( v->cfi() );
    BOOST_CHECK_EQUAL( v->vlanId(), 0x234u );
    BOOST_CHECK_EQUAL( v->type(), 0xabcd );
    BOOST_CHECK( v->next()->is<DataPacket>() );
    BOOST_CHECK_EQUAL( *v->next()->begin(), 0xf0 );
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
