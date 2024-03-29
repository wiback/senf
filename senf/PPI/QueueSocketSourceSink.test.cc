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
    \brief QueueSocketSourceSink.test unit tests */

#include "QueueSocketSourceSink.hh"

// Custom includes
#include <deque>
#include <string>
#include <boost/algorithm/string/predicate.hpp>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Socket/Protocols/Raw/TunTapSocketHandle.hh>
#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/Socket/Socket.hh>
#include <senf/PPI/FastConnector.hh>
#include <senf/PPI/Setup.hh>
#include <senf/Utils/membind.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(queueSocketSourceSink)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES(
            "Cannot test senf::ppi::module::ActiveQueueSocketSource "
            "/ senf::ppi::module::PassiveQueueSocketSink as non-root user" );

    senf::TapSocketHandle tap1;
    senf::NetdeviceController tapCtl1 (tap1.protocol().ifaceName());
    tapCtl1.up();
    senf::ConnectedMMapPacketSocketHandle pack1 (tap1.protocol().ifaceName(), 128, 128);

    senf::TapSocketHandle tap2;
    senf::NetdeviceController tapCtl2 (tap2.protocol().ifaceName());
    tapCtl2.up();
    senf::ConnectedMMapPacketSocketHandle pack2 (tap2.protocol().ifaceName(), 128, 128);

    module::ActiveQueueSocketSource<senf::EthernetPacket> source (pack1, 8);
    module::PassiveQueueSocketSink<> sink (pack2);

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

SENF_AUTO_TEST_CASE(queueSocketSourceSink_fast)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES(
            "Cannot test senf::ppi::module::ActiveQueueSocketSource "
            "/ senf::ppi::module::PassiveQueueSocketSink as non-root user" );

    senf::TapSocketHandle tap1;
    senf::NetdeviceController tapCtl1 (tap1.protocol().ifaceName());
    tapCtl1.up();
    senf::ConnectedMMapPacketSocketHandle pack1 (tap1.protocol().ifaceName(), 128, 128);

    senf::TapSocketHandle tap2;
    senf::NetdeviceController tapCtl2 (tap2.protocol().ifaceName());
    tapCtl2.up();
    senf::ConnectedMMapPacketSocketHandle pack2 (tap2.protocol().ifaceName(), 128, 128);

    module::ActiveQueueSocketSource<senf::EthernetPacket,
                                    ppi::connector::ActiveOutput<senf::EthernetPacket> > source (pack1, 8);
    module::PassiveQueueSocketSink<ppi::connector::PassiveInput<senf::EthernetPacket> > sink (pack2);

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
