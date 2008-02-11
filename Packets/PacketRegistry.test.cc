// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

// Unit tests

//#include "PacketRegistry.test.hh"
//#include "PacketRegistry.test.ih"

// Custom includes
#include <string>
#include <sstream>
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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

    namespace reg {
        PacketRegistry<StringTag>::RegistrationProxy<FooPacket> registerFoo ("foo");
        PacketRegistry<StringTag>::RegistrationProxy<BarPacket> registerBar ("bar");
    }

}

BOOST_AUTO_UNIT_TEST(packetRegistry_test)
{
    PacketRegistry<BaseTag>::registerPacket<FooPacket>(1u);
    PacketRegistry<BaseTag>::registerPacket<BarPacket>(2u);

    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<FooPacket>(), 1u );
    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<BarPacket>(), 2u );
    BOOST_CHECK_THROW( PacketRegistry<BaseTag>::key<OtherPacket>(), 
                       PacketTypeNotRegisteredException );

    BOOST_CHECK_EQUAL( PacketRegistry<StringTag>::key<FooPacket>(), "foo" );
    BOOST_CHECK( ! PacketRegistry<StringTag>::lookup("blub", senf::nothrow) );
    BOOST_CHECK( PacketRegistry<BaseTag>::lookup(1u, senf::nothrow) );

    unsigned elts1[] = { 1u, 2u };
    BOOST_CHECK_EQUAL_COLLECTIONS( PacketRegistry<BaseTag>::begin(), PacketRegistry<BaseTag>::end(),
                                   elts1+0, elts1+sizeof(elts1)/sizeof(elts1[0]) );

    std::string elts2[] = { "bar", "foo" };
    BOOST_CHECK_EQUAL_COLLECTIONS( PacketRegistry<StringTag>::begin(), PacketRegistry<StringTag>::end(),
                                   elts2+0, elts2+sizeof(elts2)/sizeof(elts2[0]) );

    std::stringstream s;
    senf::dumpPacketRegistries(s);
    BOOST_CHECK_EQUAL( s.str(),
                       "(anonymous namespace)::BaseTag:\n"
                       "1 senf::ConcretePacket<(anonymous namespace)::FooPacketType>\n"
                       "2 senf::ConcretePacket<(anonymous namespace)::BarPacketType>\n"
                       "\n"
                       "(anonymous namespace)::RegTag:\n"
                       "1 senf::ConcretePacket<(anonymous namespace)::FooPacketType>\n"
                       "2 senf::ConcretePacket<(anonymous namespace)::BarPacketType>\n"
                       "\n"
                       "(anonymous namespace)::StringTag:\n"
                       "bar senf::ConcretePacket<(anonymous namespace)::BarPacketType>\n"
                       "foo senf::ConcretePacket<(anonymous namespace)::FooPacketType>\n"
                       "\n" );

}

///////////////////////////////cc.e////////////////////////////////////////
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
