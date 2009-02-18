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

//#include "EthernetPacket.test.hh"
//#include "EthernetPacket.test.ih"

// Custom includes
#include "EthernetPacket.hh"
#include "LlcSnapPacket.hh"
#include "IPv4Packet.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ethernetPacket_parse)
{
    senf::PacketData::byte data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
        0x10, 0x11
    };                        // EtherType
    senf::EthernetPacket p (senf::EthernetPacket::create(data));

    BOOST_CHECK_EQUAL( p->destination()[3], 0x04 );
    BOOST_CHECK_EQUAL( p->source()[0], 0x07 );
    BOOST_CHECK_EQUAL( p->type_length(), 0x1011 );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

BOOST_AUTO_UNIT_TEST(ethernetPacket_parse_chain)
{
    unsigned char data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
        0x81, 0x00,                          // EtherType: VLan
        0x92, 0x34,                          // VLAN prio, cfi, id
        0xab, 0xcd,                          // EtherType
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4
    };      // Payload
    senf::EthernetPacket p (senf::EthernetPacket::create(data));

    BOOST_REQUIRE( p.next().is<senf::EthVLanPacket>() );
    senf::EthVLanPacket v (p.next().as<senf::EthVLanPacket>());
    BOOST_CHECK_EQUAL( v->priority(), 4u );
    BOOST_CHECK( v->cfi() );
    BOOST_CHECK_EQUAL( v->vlanId(), 0x234u );
    BOOST_CHECK_EQUAL( v->type(), 0xabcd );
    BOOST_CHECK( v.next().is<senf::DataPacket>() );
    BOOST_CHECK_EQUAL( *v.next().data().begin(), 0xf0 );
}

BOOST_AUTO_UNIT_TEST(ethernetPacket_create)
{
    senf::EthernetPacket eth (senf::EthernetPacket::create());
    eth->source() = senf::MACAddress::from_string("01:02:03:04:05:06");
    eth->destination() = senf::MACAddress::from_string("07:08:09:0a:0b:0c");

    senf::EthVLanPacket vlan (senf::EthVLanPacket::createAfter(eth));
    vlan->priority() = 9u;
    vlan->cfi() = true;
    vlan->vlanId() = 0x234u;

    eth.finalizeAll();
    BOOST_CHECK_EQUAL(eth->type_length(), 0x8100u);
    BOOST_CHECK_EQUAL(vlan->type(), 0u);

    senf::IPv4Packet ip (senf::IPv4Packet::createAfter(vlan));
    eth.finalizeAll();
    BOOST_CHECK_EQUAL(vlan->type(), 0x0800u);
}

BOOST_AUTO_UNIT_TEST(ethernetPacket_llcsnap)
{
    senf::EthernetPacket eth (senf::EthernetPacket::create());
    eth->source() = senf::MACAddress::from_string("01:02:03:04:05:06");
    eth->destination() = senf::MACAddress::from_string("07:08:09:0a:0b:0c");

    senf::LlcSnapPacket llcsnap (senf::LlcSnapPacket::createAfter(eth));
    senf::DataPacket payload  (senf::DataPacket::createAfter(
            llcsnap, std::string("Hello, world!")));
    eth.finalizeAll();

    BOOST_CHECK_EQUAL( eth->type_length(), 8u + 13u);
    BOOST_CHECK_EQUAL( llcsnap->dsap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ssap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ctrl(), 0x03 );
    BOOST_CHECK_EQUAL( llcsnap->protocolId(), 0x000000u );
    BOOST_CHECK_EQUAL( llcsnap->type_length(), 0u);

    senf::IPv4Packet ip (senf::IPv4Packet::createAfter(llcsnap));
    eth.finalizeAll();
    BOOST_CHECK_EQUAL(llcsnap->type_length(), 0x0800u);
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
