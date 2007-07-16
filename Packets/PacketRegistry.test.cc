// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
#include "PacketRegistry.hh"
#include "PacketType.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
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
    struct BarPacketType : public PacketTypeBase {};
    struct OtherPacketType : public PacketTypeBase {};

    namespace reg {
        PacketRegistry<StringTag>::RegistrationProxy<FooPacketType> registerFoo ("foo");
        PacketRegistry<StringTag>::RegistrationProxy<BarPacketType> registerBar ("bar");
    }

}

BOOST_AUTO_UNIT_TEST(packetRegistry_test)
{
    PacketRegistry<BaseTag>::registerPacket<FooPacketType>(1u);
    PacketRegistry<BaseTag>::registerPacket<BarPacketType>(2u);

    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<FooPacketType>(), 1u );
    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<BarPacketType>(), 2u );
    BOOST_CHECK_THROW( PacketRegistry<BaseTag>::key<OtherPacketType>(), 
                       PacketTypeNotRegisteredException );

    BOOST_CHECK_EQUAL( PacketRegistry<StringTag>::key<FooPacketType>(), "foo" );
    BOOST_CHECK( ! PacketRegistry<StringTag>::lookup("blub", senf::nothrow) );
    BOOST_CHECK( PacketRegistry<BaseTag>::lookup(1u, senf::nothrow) );

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
