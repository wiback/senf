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
    \brief EthernetPacket unit tests */

#include "EthernetPacket.hh"

// Custom includes
#include "LlcSnapPacket.hh"
#include "IPv4Packet.hh"
#include "UDPPacket.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(ethernetPacket_parse)
{
    senf::PacketData::byte data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
        0x10, 0x11
    };                        // EtherType
    senf::EthernetPacket p (senf::EthernetPacket::create(data));

    BOOST_CHECK( ! p->source().null() );
    BOOST_CHECK( ! p->source().broadcast() );
    BOOST_CHECK_EQUAL( p->destination()[3], 0x04 );
    BOOST_CHECK_EQUAL( p->source()[0], 0x07 );
    BOOST_CHECK_EQUAL( p->source(), senf::MACAddress(0x0708090a0b0c) );
    BOOST_CHECK_EQUAL( p->type_length(), 0x1011 );
    SENF_CHECK_NOT_EQUAL( p->source(), p->destination() );
    BOOST_CHECK( p->source() != senf::MACAddress::None );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));

    p->source() << senf::MACAddress::Broadcast;
    BOOST_CHECK( p->source().broadcast() );
    p->source() << senf::MACAddress::None;
    BOOST_CHECK( p->source().null() );
}

SENF_AUTO_TEST_CASE(ethernetPacket_parse_chain)
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
    BOOST_CHECK_EQUAL( v->type_length(), 0xabcd );
    BOOST_CHECK( v.next().is<senf::DataPacket>() );
    BOOST_CHECK_EQUAL( *v.next().data().begin(), 0xf0 );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_TEST_CASE(ethernetPacket_create)
{
    senf::EthernetPacket eth (senf::EthernetPacket::create());
    BOOST_CHECK( eth->source().null() );
    eth->source() = senf::MACAddress::from_string("01:02:03:04:05:06");
    eth->destination() = senf::MACAddress::from_string("07:08:09:0a:0b:0c");

    senf::EthVLanPacket vlan (senf::EthVLanPacket::createAfter(eth));
    vlan->priority() = 9u;
    vlan->cfi() = true;
    vlan->vlanId() = 0x234u;

    eth.finalizeAll();
    BOOST_CHECK_EQUAL(eth->type_length(), 0x8100u);
    BOOST_CHECK_EQUAL(vlan->type_length(), 0u);

    senf::IPv4Packet ip (senf::IPv4Packet::createAfter(vlan));
    ip->source() << senf::INet4Address::Broadcast;
    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));
    udp->source() = 42;
    eth.finalizeAll();
    BOOST_CHECK_EQUAL(vlan->type_length(), 0x0800u);

    ::memmove(
            vlan.data().begin(),
            vlan.data().begin() + senf::EthVLanPacketParser::fixed_bytes,
            vlan.size() - senf::EthVLanPacketParser::fixed_bytes);
    eth.data().resize( eth.size() - senf::EthVLanPacketParser::fixed_bytes);
    eth->type_length() = 0x0800u;
    eth.reparse();
    eth.dump(std::cout);
}

SENF_AUTO_TEST_CASE(ethernetPacket_llcsnap)
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
