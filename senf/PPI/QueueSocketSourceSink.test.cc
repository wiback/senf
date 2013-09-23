// $Id$
//
// Copyright (C) 2010
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
    \brief QueueSocketSourceSink.test unit tests */

//#include "QueueSocketSourceSink.test.hh"
//#include "QueueSocketSourceSink.test.ih"

// Custom includes
#include <deque>
#include <string>
#include <boost/algorithm/string/predicate.hpp>
#include "QueueSocketSourceSink.hh"
#include <senf/Socket/ClientSocketHandle.hh>
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Socket/Protocols/Raw/TunTapSocketHandle.hh>
#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/Socket/NetdeviceController.hh>
#include <senf/PPI/Setup.hh>
#include <senf/Utils/membind.hh>
#include <senf/Scheduler/ClockService.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace ppi = senf::ppi;

namespace {

    std::string pkgdump(senf::Packet packet)
    {
        std::stringstream ss;
        ss << "Packet data (" << packet.data().size() << " bytes)\n";
        senf::hexdump(packet.data().begin(), packet.data().end(), ss);
        std::stringstream dump;
        packet.dump(dump);
        std::string pk (dump.str());
        if (boost::algorithm::starts_with(pk, "Annotations:")) {
            unsigned i (1);
            for (; i < pk.size(); ++i)
                if (pk[i-1] == '\n' && pk[i] != ' ')
                    break;
            pk.erase(0, i);
        }
        ss << pk;
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

    template <class PacketType, class FilterType>
    struct PacketReader
    {
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy<senf::ReadablePolicy,
                                   senf::DatagramFramingPolicy,
                                   senf::ConnectedCommunicationPolicy>::policy> Handle;

        PacketReader(Handle handle, unsigned numPackets, senf::ClockService::clock_type timeout)
            : handle_ (handle),
              numPackets_ (numPackets),
              event_ ("PacketReader", senf::membind(&PacketReader::read, this), handle,
                      senf::scheduler::FdEvent::EV_READ),
              timeout_ ("PacketReader timeout", senf::membind(&PacketReader::timeout, this),
                        senf::scheduler::now() + timeout)
            {}

        void read(int events)
            {
                std::string data (handle_.read());
                if (data.empty())
                    return;
                PacketType packet (PacketType::create(data));
                if (! packet.template find<FilterType>(senf::nothrow))
                    return;
                packets.push_back(packet);
                if (packets.size() >= numPackets_)
                    senf::scheduler::terminate();
            }

        void timeout()
            {
                senf::scheduler::terminate();
            }

        std::deque<PacketType> packets;

        Handle handle_;
        unsigned numPackets_;
        senf::scheduler::FdEvent event_;
        senf::scheduler::TimerEvent timeout_;
    };

}

SENF_AUTO_UNIT_TEST(queueSocketSourceSink)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE( false,
                            "Cannot test senf::ppi::module::ActiveQueueSocketSource "
                            "/ senf::ppi::module::PassiveQueueSocketSink as non-root user" );
        BOOST_CHECK( true );
        return;
    }

    senf::TapSocketHandle tap1;
    senf::NetdeviceController tapCtl1 (tap1.protocol().ifaceName());
    tapCtl1.up();
    senf::ConnectedMMapPacketSocketHandle pack1 (tap1.protocol().ifaceName(), 128, 128);

    senf::TapSocketHandle tap2;
    senf::NetdeviceController tapCtl2 (tap2.protocol().ifaceName());
    tapCtl2.up();
    senf::ConnectedMMapPacketSocketHandle pack2 (tap2.protocol().ifaceName(), 128, 128);

    module::ActiveQueueSocketSource<senf::EthernetPacket> source (pack1, 8);
    module::PassiveQueueSocketSink sink (pack2);

    ppi::connect(source, sink);

    static char const data [] = { "0 - dead beef" };
    static std::string const strPacket (
        "Packet data (56 bytes)\n"
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
            tap1.write(eth.data());
        }
    }

    PacketReader<senf::EthernetPacket, senf::UDPPacket> reader (
        tap2, 10, senf::ClockService::seconds(1));

    senf::ppi::run();

    {
        BOOST_CHECK_EQUAL( reader.packets.size(), 10u );

        for (char ch ('0'); ! reader.packets.empty() && ch <= '9'; ++ ch) {
            BOOST_CHECK_PREDICATE( compareIgnoreChars,
                                   (pkgdump(reader.packets.front()))(tplReplace(strPacket, ch)) );

            reader.packets.pop_front();
        }
    }

#ifdef SENF_DEBUG
    BOOST_CHECK_EQUAL( source.sharedPackets(), 0u );
    BOOST_CHECK_EQUAL( source.burstMax(), 8u );
    BOOST_CHECK_EQUAL( sink.burstMax(), 8u );
#endif

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
