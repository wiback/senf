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
    \brief PacketRegistry unit tests */

//#include "PacketRegistry.test.hh"
//#include "PacketRegistry.test.ih"

// Custom includes
#include <string>
#include <sstream>
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    using namespace senf;

    struct BaseTag {
        typedef unsigned key_t;
    };

    struct StringTag {
        typedef std::string key_t;
    };

    struct FooPacketType : public PacketTypeBase {};
    typedef senf::ConcretePacket<FooPacketType> FooPacket;
    struct BarPacketType : public PacketTypeBase {};
    typedef senf::ConcretePacket<BarPacketType> BarPacket;
    struct OtherPacketType : public PacketTypeBase {};
    typedef senf::ConcretePacket<OtherPacketType> OtherPacket;

    bool contains(std::string a, std::string b)
    { return a.find(b) != std::string::npos; }

    bool contains_not(std::string a, std::string b)
    { return a.find(b) == std::string::npos; }

}

SENF_PACKET_REGISTRY_REGISTER(StringTag, "foo", OtherPacket);
SENF_PACKET_REGISTRY_REGISTER_PRIORITY(StringTag, "foo", 1, FooPacket);

SENF_AUTO_UNIT_TEST(packetRegistry_test)
{
    {
        PacketRegistry<StringTag>::registerPacket<BarPacket>("bar");
        PacketRegistry<BaseTag>::registerPacket<FooPacket>(1u);
        senf::PacketRegistry<BaseTag>::ScopedRegistrationProxy<BarPacket> registerBarInBase (2u);

        BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<FooPacket>(), 1u );
        BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<BarPacket>(), 2u );
        BOOST_CHECK_THROW( PacketRegistry<BaseTag>::key<OtherPacket>(),
                           PacketTypeNotRegisteredException );

        BOOST_CHECK_EQUAL( PacketRegistry<StringTag>::key<FooPacket>(), "foo" );
        BOOST_CHECK( PacketRegistry<StringTag>::lookup("foo").type() == typeid(FooPacket) );
        BOOST_CHECK( ! PacketRegistry<StringTag>::lookup("blub", senf::nothrow) );
        BOOST_CHECK( PacketRegistry<BaseTag>::lookup(1u, senf::nothrow) );

        unsigned elts1[] = { 1u, 2u };
        BOOST_CHECK_EQUAL_COLLECTIONS(
            boost::make_transform_iterator(
                PacketRegistry<BaseTag>::begin(),
                boost::multi_index::member<senf::PacketRegistry<BaseTag>::Entry, unsigned,
                                           &senf::PacketRegistry<BaseTag>::Entry::key>()),
            boost::make_transform_iterator(
                PacketRegistry<BaseTag>::end(),
                boost::multi_index::member<senf::PacketRegistry<BaseTag>::Entry, unsigned,
                                           &senf::PacketRegistry<BaseTag>::Entry::key>()),
            elts1+0, elts1+sizeof(elts1)/sizeof(elts1[0]) );

        std::string elts2[] = { "bar", "foo", "foo" };
        BOOST_CHECK_EQUAL_COLLECTIONS(
            boost::make_transform_iterator(
                PacketRegistry<StringTag>::begin(),
                boost::multi_index::member<senf::PacketRegistry<StringTag>::Entry, std::string,
                                           &senf::PacketRegistry<StringTag>::Entry::key>()),
            boost::make_transform_iterator(
                PacketRegistry<StringTag>::end(),
                boost::multi_index::member<senf::PacketRegistry<StringTag>::Entry, std::string,
                                           &senf::PacketRegistry<StringTag>::Entry::key>()),
            elts2+0, elts2+sizeof(elts2)/sizeof(elts2[0]) );

        std::stringstream s;
        senf::dumpPacketRegistries(s);
        BOOST_CHECK_PREDICATE(
            contains,
            (s.str())
            ("(anonymous namespace)::BaseTag:\n"
             "  0x00000001 (         1) (....)      0 (anonymous namespace)::FooPacketType\n"
             "  0x00000002 (         2) (....)      0 (anonymous namespace)::BarPacketType\n"
             "\n"));
        BOOST_CHECK_PREDICATE(
            contains,
            (s.str())
            ("(anonymous namespace)::StringTag:\n"
             "  bar                   0 (anonymous namespace)::BarPacketType\n"
             "  foo                   1 (anonymous namespace)::FooPacketType\n"
             "  foo                   0 (anonymous namespace)::OtherPacketType\n"
             "\n" ));
    }

    {
        std::stringstream s;
        senf::dumpPacketRegistries(s);
        BOOST_CHECK_PREDICATE(
            contains,
            (s.str())
            ("(anonymous namespace)::BaseTag:\n"
             "  0x00000001 (         1) (....)      0 (anonymous namespace)::FooPacketType\n"
             "\n"));
        BOOST_CHECK_PREDICATE(
            contains,
            (s.str())
            ("(anonymous namespace)::StringTag:\n"
             "  bar                   0 (anonymous namespace)::BarPacketType\n"
             "  foo                   1 (anonymous namespace)::FooPacketType\n"
             "  foo                   0 (anonymous namespace)::OtherPacketType\n"
             "\n" ));

        SENF_CHECK_NO_THROW( PacketRegistry<BaseTag>::unregisterPacket(1u) );
        SENF_CHECK_NO_THROW( PacketRegistry<StringTag>::unregisterPacket<BarPacket>() );

        s.str("");
        senf::dumpPacketRegistries(s);
        BOOST_CHECK_PREDICATE(
            contains_not,
            (s.str())
            ("(anonymous namespace)::BaseTag:\n"));
        BOOST_CHECK_PREDICATE(
            contains,
            (s.str())
            ("(anonymous namespace)::StringTag:\n"
             "  foo                   1 (anonymous namespace)::FooPacketType\n"
             "  foo                   0 (anonymous namespace)::OtherPacketType\n"
             "\n" ));
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
