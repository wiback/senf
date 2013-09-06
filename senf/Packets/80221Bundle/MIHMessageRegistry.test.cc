// $Id$
//
// Copyright (C) 2011
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief MIH Message-Registry unit tests */

//#include "MIHPacket.test.hh"
//#include "MIHPacket.test.ih"

// Custom includes
#include "MIHMessageRegistry.hh"
#include "MIHPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

        static const boost::uint16_t MESSAGE_ID;
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

        static const boost::uint16_t MESSAGE_ID;

        static void validate(packet message) {
            message->registerRequestCodeTLV().validate();
        }
    };

    typedef senf::ConcretePacket<TestMessagePacketType> TestMessagePacket;
    typedef senf::ConcretePacket<ValidatedTestMessagePacketType> ValidatedTestMessagePacket;

    const boost::uint16_t TestMessagePacketType::MESSAGE_ID = 42;
    const boost::uint16_t ValidatedTestMessagePacketType::MESSAGE_ID = 43;
}
}

using namespace senf;

SENF_MIH_PACKET_REGISTRY_REGISTER( test::TestMessagePacket );
SENF_MIH_PACKET_REGISTRY_REGISTER( test::ValidatedTestMessagePacket );


SENF_AUTO_UNIT_TEST(MIHMessageRegistry_validate)
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
    mihPacket->messageId() = test::TestMessagePacketType::MESSAGE_ID;
    SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));

    {
        test::TestMessagePacket testMessage (test::TestMessagePacket::createAfter(mihPacket));
        mihPacket.finalizeAll();
        SENF_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
        mihPacket->messageId() = test::ValidatedTestMessagePacketType::MESSAGE_ID;
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
