// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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

/** \file
    \brief Packet.test unit tests */

//#include "Packet.test.hh"
//#include "Packet.test.ih"

// Custom includes
#include <sstream>
#include "Packets.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct RegTag {
        typedef unsigned key_t;
    };

    struct FooPacketType 
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<FooPacketType>
    {
        using senf::PacketTypeMixin<FooPacketType>::nextPacketRange;
        using senf::PacketTypeMixin<FooPacketType>::initSize;
        using senf::PacketTypeMixin<FooPacketType>::init;
        typedef senf::PacketInterpreter<FooPacketType> interpreter;
        static interpreter::size_type initSize()
            { return 4u; }
    };
    typedef senf::ConcretePacket<FooPacketType> FooPacket;

    struct BarPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        
        SENF_PARSE_FIELD( type,     senf::Parse_UInt16 );
        SENF_PARSE_FIELD( length,   senf::Parse_Int32  );
        SENF_PARSE_FIELD( reserved, senf::Parse_UInt16 );

        SENF_PARSER_FINALIZE(BarPacketParser);
    };

    struct BarPacketType 
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<BarPacketType,RegTag>
    {
        typedef senf::PacketTypeMixin<BarPacketType,RegTag> mixin;
        typedef senf::ConcretePacket<BarPacketType> packet;
        typedef BarPacketParser parser;
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;
        static size_type initSize() 
            { return 8u; }
        static void init(packet p) {
            p->reserved() = 0xA0A0u;
        }
        static void dump(packet p, std::ostream & os) {
            os << "BarPacket:\n"
               << "type: " << p->type() << "\n"
               << "length: " << p->length() << "\n";
        }
        static void finalize(packet p) {
            if (p.next())
                p->type() = senf::PacketRegistry<RegTag>::key(p.next());
            else
                p->type() = -1;
        }
        static registry_key_t nextPacketKey(packet p) {
            return p->type();
        }
    };
    typedef BarPacketType::packet BarPacket;

    namespace reg {
        senf::PacketRegistry<RegTag>::RegistrationProxy<FooPacket> registerFoo(1u);
        senf::PacketRegistry<RegTag>::RegistrationProxy<BarPacket> registerBar(2u);
    }

}

BOOST_AUTO_UNIT_TEST(packet)
{
    senf::Packet packet (FooPacket::create());
    BarPacket::createAfter(packet);

    BOOST_REQUIRE( packet );
    BOOST_CHECK( packet.next() );
    BOOST_CHECK( ! packet.next().next() );
    BOOST_CHECK( ! packet.prev() );
    BOOST_CHECK( packet.next().prev() == packet );
    BOOST_CHECK( packet.next() != packet );
    BOOST_CHECK_EQUAL( packet.size(), 12u );
    BOOST_CHECK_EQUAL( packet.next().size(), 8u );
    BOOST_CHECK( packet.is<FooPacket>() );
    BOOST_CHECK( packet.next().is<BarPacket>() );
    BOOST_CHECK( packet.first() == packet );
    BOOST_CHECK( packet.last() == packet.next() );
    
    senf::Packet p2 (packet.next());
    BOOST_CHECK( p2 );
    packet.parseNextAs<FooPacket>();
    BOOST_CHECK_EQUAL( packet.size(), 12u );
    BOOST_CHECK_EQUAL( packet.next().size(), 8u );
    BOOST_CHECK( packet.next().is<FooPacket>() );
    BOOST_CHECK( ! p2 );
    BOOST_CHECK( packet.next().as<FooPacket>() );
    
    p2 = packet.next().clone();
    BOOST_REQUIRE( p2 );
    packet.next().append( p2 );
    BOOST_REQUIRE( packet.next().next() );
    BOOST_CHECK( packet.next().next().next() );
    BOOST_CHECK( packet.next().next().next().is<senf::DataPacket>() );
    BOOST_CHECK_EQUAL( packet.size(), 16u );

    // This calls and checks typeId()
    BOOST_CHECK_EQUAL( senf::PacketRegistry<RegTag>::key(packet), 1u );
    packet.next().parseNextAs( senf::PacketRegistry<RegTag>::lookup(2u).factory() );
    BOOST_CHECK( packet.next().next().is<BarPacket>() );
    
    std::stringstream s;
    packet.dump(s);
    BOOST_CHECK_EQUAL( s.str(), "BarPacket:\ntype: 0\nlength: 0\n" );
    
    packet.finalize();
    BOOST_CHECK_EQUAL( packet.last().as<BarPacket>()->type(), 
                       BarPacket::type::parser::type_t::value_type(-1) );
    packet.last().append(FooPacket::create());
    packet.finalize();
    BOOST_CHECK_EQUAL( packet.next<BarPacket>()->type(), 1u );

    BOOST_CHECK( packet.factory() == FooPacket::factory() );

    senf::PacketData::byte data[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x81, 0x82, 0x83 };

    BarPacket::createAfter(packet,data);
    BOOST_REQUIRE( packet.next() );
    BOOST_REQUIRE( packet.next().is<BarPacket>() );
    BOOST_CHECK( packet.last().is<FooPacket>() );
    BOOST_CHECK_EQUAL( packet.last<BarPacket>()->type(), 1u );
    BOOST_CHECK_EQUAL( packet.next().size(), 11u );
    BOOST_REQUIRE( packet.next().next() );
    BOOST_CHECK( packet.next().next().is<FooPacket>() );
    BOOST_CHECK( ! packet.next().next().next() );
    BOOST_CHECK_EQUAL( packet.next().next().data()[0], 0x81u );

    BOOST_CHECK( packet.first<FooPacket>() == packet );
    BOOST_CHECK( packet.first<FooPacket>(senf::nothrow) == packet );
    BOOST_CHECK( packet.last<BarPacket>() == packet.last().prev() );
    BOOST_CHECK( packet.last<BarPacket>(senf::nothrow) == packet.last().prev() );
    BOOST_CHECK( packet.findNext<FooPacket>() == packet );
    BOOST_CHECK( packet.findNext<FooPacket>(senf::nothrow) == packet );
    BOOST_CHECK( packet.last().findPrev<FooPacket>() == packet.last() );
    BOOST_CHECK( packet.last().findPrev<FooPacket>(senf::nothrow) == packet.last() );
    BOOST_CHECK( packet.next<BarPacket>() == packet.next() );
    BOOST_CHECK( packet.next<BarPacket>(senf::nothrow) == packet.next() );
    BOOST_CHECK( packet.last().prev<FooPacket>() == packet );
    BOOST_CHECK( packet.last().prev<FooPacket>(senf::nothrow) == packet );
}

BOOST_AUTO_UNIT_TEST(concretePacket)
{
    senf::PacketData::byte data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

    BOOST_CHECK_EQUAL( FooPacket::create().size(), 4u );
    BOOST_CHECK_EQUAL( FooPacket::create(FooPacket::noinit).size(), 0u );
    BOOST_CHECK_THROW( FooPacket::create(2u), senf::TruncatedPacketException );
    BOOST_CHECK_EQUAL( FooPacket::create(10u).size(), 10u );
    BOOST_CHECK_EQUAL( FooPacket::create(2u,FooPacket::noinit).size(), 2u );
    BOOST_CHECK_EQUAL( FooPacket::create(data).size(), 6u );

    senf::Packet packet (FooPacket::create());

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet).size(), 4u );
    BOOST_CHECK_EQUAL( packet.size(), 8u );

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,FooPacket::noinit).size(), 0u );
    BOOST_CHECK_EQUAL( packet.size(), 4u );

    BOOST_CHECK_THROW( FooPacket::createAfter(packet,2u), senf::TruncatedPacketException );
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,10u).size(), 10u );
    BOOST_CHECK_EQUAL( packet.size(), 14u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,2u,FooPacket::noinit).size(), 2u );
    BOOST_CHECK_EQUAL( packet.size(), 6u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,data).size(), 6u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );
    
    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet).size(), 14u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );

    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet,FooPacket::noinit).size(), 10u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );

    BOOST_CHECK( packet.clone() != packet );
    BOOST_CHECK_EQUAL( BarPacket::create()->reserved(), 0xA0A0u );
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
