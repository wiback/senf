// $Id$
//
// Copyright (C) 2006
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
    \brief PacketSocketHandle unit tests */

//#include "PacketSocketHandle.test.hh"
//#include "PacketSocketHandle.test.ih"

// Custom includes
#include <iostream>
#include <unistd.h>
#include "PacketSocketHandle.hh"
#include "TunTapSocketHandle.hh"
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(packetSocketHandle)
{
    // We have a Problem here .. packet sockets are only allowed for root
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::PacketSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

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
