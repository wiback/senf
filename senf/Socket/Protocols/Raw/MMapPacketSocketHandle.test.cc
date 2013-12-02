// $Id$
//
// Copyright (C) 2013
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
    \brief MMapPacketSocketHandle.test unit tests */

//#include "MMapPacketSocketHandle.test.hh"
//#include "MMapPacketSocketHandle.test.ih"

// Custom includes
#include <sstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>
#include "MMapPacketSocketHandle.hh"
#include "TunTapSocketHandle.hh"
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/String.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    std::string pkgdump(senf::Packet packet)
    {
        std::stringstream ss;
        senf::hexdump(packet.data().begin(), packet.data().end(), ss);
        packet.dump(ss);
        return ss.str();
    }

    std::string tplReplace(std::string tpl, char code)
    {
        std::string res (tpl);
        boost::replace_all(res, std::string("##"), (boost::format("%02x") % unsigned(code)).str());
        boost::replace_all(res, std::string("#"), std::string(1, code));
        return res;
    }

    bool compareIgnoreChars(std::string a, std::string b)
    {
        if (a.size() != b.size())
            return false;
        for (unsigned i (0); i < a.size(); ++i)
            if (a[i] != '?' && b[i] != '?' && a[i] != b[i])
                return false;
        return true;
    }



    template <class Handle>
    void runTest(senf::TapSocketHandle tap, Handle pk)
    {
        static char const data [] = { "0 - dead beef" };
        static std::string const strPacket (
            "  0000  02 03 04 05 06 07 01 02  03 04 05 06 08 00 45 00  ........ ......E.\n"
            "  0010  00 2a 00 00 00 00 40 11  ?? ?? 7f 00 00 01 7f 00  .*....@. |.......\n"
            "  0020  00 01 13 89 13 89 00 16  ?? ?? ## 20 2d 20 64 65  ........ ..# - de\n"
            "  0030  61 64 20 62 65 65 66 00                           ad beef.\n"
            "Ethernet II (DIX): \n"
            "  destination             : 02:03:04:05:06:07\n"
            "  source                  : 01:02:03:04:05:06\n"
            "  type/length             : 0x0800 ( 2048) (..)\n"
            "Internet protocol Version 4:\n"
            "  version                 : 4\n"
            "  ip header length        : 5\n"
            "  tos                     : 0\n"
            "  length                  : 42\n"
            "  identifier              : 0\n"
            "  dont fragment           : 0\n"
            "  more fragments          : 0\n"
            "  fragment                : 0\n"
            "  ttl                     : 64\n"
            "  protocol                : 17\n"
            "  checksum                : 0x????\n"
            "  source                  : 127.0.0.1\n"
            "  destination             : 127.0.0.1\n"
            "UDP:\n"
            "  source port             : 5001\n"
            "  dest port               : 5001\n"
            "  length                  : 22\n"
            "  checksum                : 0x????\n"
            "Payload data (14 bytes)\n");

        {
            senf::LLSocketAddress a;
            SENF_CHECK_NO_THROW( pk.local(a) );
            BOOST_CHECK_EQUAL( a.interface(), tap.protocol().ifaceName() );

            SENF_CHECK_NO_THROW( pk.protocol().mcAdd(
                                     tap.protocol().ifaceName(),
                                     senf::MACAddress::from_string("01-02-03-04-05-06")) );
            SENF_CHECK_NO_THROW( pk.protocol().mcDrop(
                                     tap.protocol().ifaceName(),
                                     senf::MACAddress::from_string("01-02-03-04-05-06")) );

            SENF_CHECK_NO_THROW( pk.protocol().promisc( tap.protocol().ifaceName(), true) );
            SENF_CHECK_NO_THROW( pk.protocol().promisc( tap.protocol().ifaceName(), false));

            SENF_CHECK_NO_THROW( pk.protocol().available() );
            BOOST_CHECK( ! pk.eof() );
        }

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

            for (char ch ('0'); ch <= '9'; ++ ch) {
                * dp.data().begin() = ch;
                eth.finalizeAll();
                tap.write(eth.data());
            }
        }

        {
            for (char ch ('0'); ch <= '9'; ++ ch) {
                senf::EthernetPacket eth;
                SENF_REQUIRE_NO_THROW( eth = senf::EthernetPacket::create(pk.read()) );

                BOOST_CHECK_PREDICATE( compareIgnoreChars,
                                       (pkgdump(eth))(tplReplace(strPacket, ch)) );
            }
        }

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

            for (char ch ('0'); ch <= '9'; ++ ch) {
                * dp.data().begin() = ch;
                eth.finalizeAll();
                pk.write(eth.data());
            }
        }

        {
            for (char ch ('0'); ch <= '9'; ++ ch) {
                for (;;) {
                    senf::EthernetPacket eth;
                    SENF_REQUIRE_NO_THROW( eth = senf::EthernetPacket::create(tap.read()) );
                    if (eth.find<senf::IPv6Packet>(senf::nothrow))
                        // IPv6 Router solicitation
                        continue;
                    senf::DataPacket dp;
                    SENF_REQUIRE_NO_THROW( dp = eth.find<senf::DataPacket>() );

                    BOOST_CHECK_PREDICATE( compareIgnoreChars,
                                           (pkgdump(eth))(tplReplace(strPacket, ch)) );
                    break;
                }
            }
        }
    }

}

SENF_AUTO_UNIT_TEST(connectedMMapPacketSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::MMapPacketSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

    senf::TapSocketHandle tap;
    senf::NetdeviceController tapCtl (tap.protocol().ifaceName());
    tapCtl.up();
    senf::ConnectedMMapPacketSocketHandle pk (tap.protocol().ifaceName(), 128);

    runTest(tap, pk);
}

SENF_AUTO_UNIT_TEST(connectedMMapReadPacketSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::MMapPacketSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

    senf::TapSocketHandle tap;
    senf::NetdeviceController tapCtl (tap.protocol().ifaceName());
    tapCtl.up();
    senf::ConnectedMMapReadPacketSocketHandle pk (tap.protocol().ifaceName(), 128);

    runTest(tap, pk);
}

SENF_AUTO_UNIT_TEST(connectedMMapWritePacketSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::MMapPacketSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

    senf::TapSocketHandle tap;
    senf::NetdeviceController tapCtl (tap.protocol().ifaceName());
    tapCtl.up();
    senf::ConnectedMMapWritePacketSocketHandle pk (tap.protocol().ifaceName(), 128);

    runTest(tap, pk);
}

SENF_AUTO_UNIT_TEST(connectedPacketSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::MMapPacketSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

    senf::TapSocketHandle tap;
    senf::NetdeviceController tapCtl (tap.protocol().ifaceName());
    tapCtl.up();
    senf::ConnectedPacketSocketHandle pk (tap.protocol().ifaceName());

    runTest(tap, pk);
}

///////////////////////////////cc.e////////////////////////////////////////
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
