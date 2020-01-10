//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include "EthernetFragmentation.hh"

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/PPI/Setup.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
namespace {
    unsigned UniqueNumber() {
        static unsigned current = 0;
        return ++current;
    }

    bool IsUniqueNumber(senf::PacketData::byte b) {
        static unsigned current = 1;
        return b == ++current;
    }

    void checkFragment(senf::EthernetPacket eth, unsigned size, unsigned fragmentNr, bool moreFragment) {
        BOOST_CHECK_EQUAL( eth->destination(), senf::MACAddress::Broadcast);
        senf::Packet ethPayload (eth.next());
        if (eth->type_length() == senf::EthVLanPacketType::etherType)
            ethPayload = ethPayload.next();
        BOOST_CHECK_EQUAL( ethPayload.size(), size);
        senf::emu::EthernetFragmentPacket fragment (eth.find<senf::emu::EthernetFragmentPacket>());
        BOOST_REQUIRE( fragment);
        BOOST_CHECK_EQUAL( fragment->fragmentNr(), fragmentNr++);
        BOOST_CHECK_EQUAL( fragment->moreFragment(), moreFragment);
        BOOST_CHECK_EQUAL( fragment->type_length(), 42);
        senf::PacketData & payload (fragment.next().data());
        BOOST_CHECK( std::find_if (payload.begin(), payload.end(), IsUniqueNumber) == payload.end() );
    }

    struct TestAnnotation
    {
        unsigned value;
        TestAnnotation() {}
        bool operator<(TestAnnotation const & other) const {
            return value < other.value;
        }
    };
    std::ostream & operator<<(std::ostream & os, TestAnnotation const & annotation) {
        os << annotation.value;
        return os;
    }
}

SENF_AUTO_TEST_CASE(Eth_Fragmenter)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::emu::EthernetFragmenter fragmenter (150);
    senf::ppi::module::debug::PassiveSink sink;

    senf::ppi::connect( source, fragmenter);
    senf::ppi::connect( fragmenter, sink);
    senf::ppi::init();

    senf::EthernetPacket ethPacket (senf::EthernetPacket::create());
    ethPacket->destination() << senf::MACAddress::Broadcast;
    ethPacket->type_length() << 42;
    senf::DataPacket::createAfter( ethPacket, 100);
    {
        ethPacket.finalizeAll();
        source.submit( ethPacket);

        BOOST_REQUIRE_EQUAL( sink.size(), 1);
        senf::EthernetPacket p2 (sink.pop_front().as<senf::EthernetPacket>());
        SENF_CHECK_EQUAL_COLLECTIONS(
                ethPacket.data().begin(), ethPacket.data().end(),
                p2.data().begin(), p2.data().end() );
    }
    senf::DataPacket payload (senf::DataPacket::createAfter( ethPacket, 151));
    std::generate( payload.data().begin(), payload.data().end(), UniqueNumber);

    ethPacket.finalizeAll();
    source.submit( ethPacket);

    BOOST_REQUIRE_EQUAL( sink.size(), 2);

    BOOST_CHECK_EQUAL( sink.front().find<senf::emu::EthernetFragmentPacket>()->size(), payload.size());
    BOOST_MESSAGE( "check eth fragment 1");
    checkFragment( sink.pop_front().as<senf::EthernetPacket>(), fragmenter.fragmentationThreshold(), 1, true);
    BOOST_MESSAGE( "check eth fragment 2");
    checkFragment( sink.pop_front().as<senf::EthernetPacket>(),
            64 - senf::EthernetPacketParser::fixed_bytes, 2, false);

    senf::EthVLanPacket vlanPacket (senf::EthVLanPacket::createInsertBefore( payload));
    vlanPacket->type_length() << 42;
    ethPacket.finalizeAll();

    source.submit( ethPacket);

    BOOST_REQUIRE_EQUAL( sink.size(), 2);

    BOOST_MESSAGE( "check eth vlan fragment 1");
    BOOST_CHECK_EQUAL( sink.front().find<senf::emu::EthernetFragmentPacket>()->size(), payload.size());
    checkFragment( sink.pop_front().as<senf::EthernetPacket>(), fragmenter.fragmentationThreshold(), 1, true);
    BOOST_MESSAGE( "check eth vlan fragment 2");
    checkFragment( sink.pop_front().as<senf::EthernetPacket>(),
            64 - senf::EthernetPacketParser::fixed_bytes - senf::EthVLanPacketParser::fixed_bytes, 2, false);
}

SENF_AUTO_TEST_CASE(Eth_Reassembler)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::emu::EthernetReassembler reassembler;
    senf::ppi::module::debug::PassiveSink sink;

    senf::ppi::connect( source, reassembler);
    senf::ppi::connect( reassembler, sink);
    senf::ppi::init();

    senf::EthernetPacket ethPacket (senf::EthernetPacket::create());
    ethPacket->destination() << senf::MACAddress::Broadcast;
    ethPacket->type_length() << 42;

    {
        ethPacket.finalizeAll();
        source.submit( ethPacket);

        BOOST_REQUIRE_EQUAL( sink.size(), 1);
        senf::EthernetPacket p2 (sink.pop_front().as<senf::EthernetPacket>());
        SENF_CHECK_EQUAL_COLLECTIONS(
                ethPacket.data().begin(), ethPacket.data().end(),
                p2.data().begin(), p2.data().end() );
    }
}

namespace {
    class EthernetSizeChecker
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE( EthernetSizeChecker );

        unsigned mtu_;

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetSizeChecker(unsigned mtu) : mtu_(mtu) {
            route( input, output);
            input.onRequest( &EthernetSizeChecker::onRequest);
        }

        void onRequest() {
            senf::EthernetPacket packet (input.read());
            BOOST_CHECK( packet.size() >= 64);
            senf::Packet::size_type payloadSize (packet.size() - senf::EthernetPacketParser::fixed_bytes);
            if (packet->type_length() == senf::EthVLanPacketType::etherType)
                payloadSize -= senf::EthVLanPacketParser::fixed_bytes;
            BOOST_CHECK( payloadSize <= mtu_);
            output( packet);
        }
    };
}

SENF_AUTO_TEST_CASE(Eth_Fragmentation)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::emu::EthernetFragmenter fragmenter (150);
    EthernetSizeChecker sizeChecker (fragmenter.fragmentationThreshold());
    senf::emu::EthernetReassembler reassembler;
    senf::ppi::module::debug::PassiveSink sink;

    senf::ppi::connect( source, fragmenter);
    senf::ppi::connect( fragmenter, sizeChecker);
    senf::ppi::connect( sizeChecker, reassembler);
    senf::ppi::connect( reassembler, sink);
    senf::ppi::init();

    senf::EthernetPacket ethPacket (senf::EthernetPacket::create());
    ethPacket->destination() << senf::MACAddress::Broadcast;
    ethPacket.annotation<TestAnnotation>().value = 42;

    senf::DataPacket payload (senf::DataPacket::createAfter( ethPacket, fragmenter.fragmentationThreshold() + 1));
    std::generate( payload.data().begin(), payload.data().end(), UniqueNumber);

    ethPacket.finalizeAll();
    source.submit( ethPacket);

    senf::EthernetPacket p2 (sink.pop_front().as<senf::EthernetPacket>());
    SENF_CHECK_EQUAL_COLLECTIONS(
            ethPacket.data().begin(), ethPacket.data().end(),
            p2.data().begin(), p2.data().end() );
    BOOST_CHECK_EQUAL( ethPacket.annotation<TestAnnotation>().value,
            p2.annotation<TestAnnotation>().value );

    senf::EthVLanPacket vlanPacket (senf::EthVLanPacket::createInsertBefore( payload));
    vlanPacket->vlanId() << 23;
    vlanPacket->type_length() << 42;

    ethPacket.finalizeAll();
    source.submit( ethPacket);

    senf::EthernetPacket p3 (sink.pop_front().as<senf::EthernetPacket>());
    SENF_CHECK_EQUAL_COLLECTIONS(
            ethPacket.data().begin(), ethPacket.data().end(),
            p3.data().begin(), p3.data().end() );
    BOOST_CHECK_EQUAL( ethPacket.annotation<TestAnnotation>().value,
            p3.annotation<TestAnnotation>().value );

    {
        senf::DataPacket data (senf::DataPacket::createAfter( ethPacket, 10));
        std::generate( data.data().begin(), data.data().end(), UniqueNumber);
        ethPacket->type_length() << 42;

        source.submit( ethPacket);

        senf::EthernetPacket p4 (sink.pop_front().as<senf::EthernetPacket>());
        SENF_CHECK_EQUAL_COLLECTIONS(
                ethPacket.data().begin(), ethPacket.data().end(),
                p4.data().begin(), p4.data().end() );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
