// $Id$
//
// Copyright (C) 2011
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
            return message->registerRequestCodeTLV().validate();
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

    BOOST_CHECK_THROW( MIHPacketType::validate( mihPacket), InvalidMIHPacketException);

    mihPacket.finalizeThis();
    BOOST_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));

    {
        test::TestMessagePacket testMessage (test::TestMessagePacket::createAfter(mihPacket));
        mihPacket.finalizeAll();
        BOOST_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
    }
    {
        test::ValidatedTestMessagePacket testMessage (test::ValidatedTestMessagePacket::createAfter(mihPacket));
        mihPacket.finalizeAll();
        testMessage->registerRequestCodeTLV().value() << 3;
        BOOST_CHECK_THROW( MIHPacketType::validate( mihPacket), InvalidMIHPacketException);
        testMessage->registerRequestCodeTLV().value() << 1;
        BOOST_CHECK_NO_THROW( MIHPacketType::validate( mihPacket));
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
