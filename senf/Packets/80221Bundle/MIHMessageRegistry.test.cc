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
    \brief MIH Message-Registry unit tests */

#include "MIHMessageRegistry.hh"

// Custom includes
#include "MIHPacket.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

// we cannot put these test classes into the anonymous namespace because of a g++ bug
// see https://svn.boost.org/trac/boost/ticket/3729

namespace senf {
namespace test {

    class TestMessagePacketParser
        : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_FIELD ( registerRequestCodeTLV, senf::MIHRegisterReqCodeTLVParser );
        SENF_PARSER_FINALIZE ( TestMessagePacketParser );
    };


    struct TestMessagePacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TestMessagePacketType>
    {
        typedef senf::ConcretePacket<TestMessagePacketType> packet;
        typedef senf::PacketTypeMixin<TestMessagePacketType> mixin;
        typedef TestMessagePacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static const boost::uint16_t messageId;
    };

    struct ValidatedTestMessagePacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<ValidatedTestMessagePacketType>
    {
        typedef senf::ConcretePacket<ValidatedTestMessagePacketType> packet;
        typedef senf::PacketTypeMixin<ValidatedTestMessagePacketType> mixin;
        typedef TestMessagePacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static const boost::uint16_t messageId;

        static void validate(packet message) {
            message->registerRequestCodeTLV().validate();
        }
    };

    typedef senf::ConcretePacket<TestMessagePacketType> TestMessagePacket;
    typedef senf::ConcretePacket<ValidatedTestMessagePacketType> ValidatedTestMessagePacket;

    const boost::uint16_t TestMessagePacketType::messageId = 42;
    const boost::uint16_t ValidatedTestMessagePacketType::messageId = 43;
}
}

using namespace senf;

SENF_MIH_PACKET_REGISTRY_REGISTER( test::TestMessagePacket, 42);
SENF_MIH_PACKET_REGISTRY_REGISTER( test::ValidatedTestMessagePacket, 43 );


SENF_AUTO_TEST_CASE(MIHMessageRegistry_validate)
{
    MIHPacket mihPacket (MIHPacket::create());
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( "senf@berlios.de");
    mihPacket->dst_mihfId().value( "test");

    mihPacket.finalizeThis();
    SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
    mihPacket.data().resize(mihPacket.size()-1);
    BOOST_CHECK_THROW( MIHPacketType::validate( mihPacket), InvalidMIHPacketException);
    mihPacket.data().resize(mihPacket.size()+2);
    mihPacket->messageId() = test::TestMessagePacketType::messageId;
    SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));

    {
        test::TestMessagePacket testMessage (test::TestMessagePacket::createAfter(mihPacket));
        mihPacket.finalizeAll();
        SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
        mihPacket->messageId() = test::ValidatedTestMessagePacketType::messageId;
        BOOST_CHECK_THROW( MIHPacketType::validate( mihPacket), InvalidMIHPacketException);
    }
    {
        test::ValidatedTestMessagePacket testMessage (test::ValidatedTestMessagePacket::createAfter(mihPacket));
        mihPacket.finalizeAll();
        testMessage->registerRequestCodeTLV().value() << 3;
        BOOST_CHECK_THROW( MIHPacketType::validate( mihPacket), InvalidMIHPacketException);
        testMessage->registerRequestCodeTLV().value() << 1;
        SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
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
