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
    \brief PacketSocketHandle unit tests */

#include "PacketSocketHandle.hh"

// Custom includes
#include <unistd.h>
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include "TunTapSocketHandle.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(packetSocketHandle)
{
    // We have a Problem here .. packet sockets are only allowed for root
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::PacketSocketHandle as non-root user");

    senf::TapSocketHandle tap;
    senf::NetdeviceController tapCtl (tap.protocol().ifaceName());
    tapCtl.up();

    {
        senf::PacketSocketHandle sock;

        SENF_CHECK_NO_THROW( sock.bind(senf::LLSocketAddress(tap.protocol().ifaceName())) );
        senf::LLSocketAddress a;
        SENF_CHECK_NO_THROW( sock.local(a) );
        BOOST_CHECK_EQUAL( a.interface(), tap.protocol().ifaceName() );

        SENF_CHECK_NO_THROW( sock.protocol().mcAdd(
                                 tap.protocol().ifaceName(),
                                 senf::MACAddress::from_string("01-02-03-04-05-06")) );
        SENF_CHECK_NO_THROW( sock.protocol().mcDrop(
                                 tap.protocol().ifaceName(),
                                 senf::MACAddress::from_string("01-02-03-04-05-06")) );

        SENF_CHECK_NO_THROW( sock.protocol().promisc( tap.protocol().ifaceName(), true) );
        SENF_CHECK_NO_THROW( sock.protocol().promisc( tap.protocol().ifaceName(), false));

        SENF_CHECK_NO_THROW( sock.protocol().available() );
        BOOST_CHECK( ! sock.eof() );
    }

    senf::PacketSocketHandle pk;
    pk.bind(senf::LLSocketAddress(tap.protocol().ifaceName()));

    static char const data [] = { "dead beef" };

    {
        senf::EthernetPacket eth (senf::EthernetPacket::create());
        eth->source() << senf::MACAddress::from_string("01-02-03-04-05-06");
        eth->destination() << senf::MACAddress::from_string("02-03-04-05-06-07");

        senf::IPv4Packet ip (senf::IPv4Packet::createAfter(eth));
        ip->source() << senf::INet4Address::from_string("127.0.0.1");
        ip->destination() << senf::INet4Address::from_string("127.0.0.1");
        ip->ttl() << 64;

        senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));
        udp->source() << 5001;
        udp->destination() << 5001;

        senf::DataPacket dp (senf::DataPacket::createAfter(
                                 udp,
                                 boost::make_iterator_range(data, data + sizeof(data))));

        eth.finalizeAll();

        tap.write(eth.data());
    }

    {
        senf::EthernetPacket eth (senf::EthernetPacket::create(pk.read()));
        senf::DataPacket dp (eth.find<senf::DataPacket>());

        BOOST_CHECK_EQUAL( std::string(dp.data().begin(), dp.data().end()),
                           std::string(data, sizeof(data)) );
    }
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
