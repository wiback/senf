// $Id$
//
// Copyright (C) 2007
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

/** \file
    \brief Packet.test unit tests */

//#include "Packet.test.hh"
//#include "Packet.test.ih"

// Custom includes
#include <sstream>
#include <boost/static_assert.hpp>
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
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
        static size_type initSize()
            { return 4u; }

        // We need to implement initHeadSize() to force the mixin to switch into 'fixed-size'
        // mode. Otherwise, mixin::nextPacketRange() would query the parser for it's size to find
        // the header size. Since the parser is VoidPacketParser, the header size would therefore be
        // 0
        static size_type initHeadSize() 
            { return initSize(); }
    };
    typedef senf::ConcretePacket<FooPacketType> FooPacket;

    struct BarPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        
        SENF_PARSER_FIELD( type,     senf::UInt16Parser );
        SENF_PARSER_FIELD( length,   senf::Int32Parser  );
        SENF_PARSER_FIELD( reserved, senf::UInt16Parser );

        SENF_PARSER_INIT() {
            reserved() << 0xA0A0u;
        }

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
        static void dump(packet p, std::ostream & os) {
            os << "BarPacket:\n"
               << "type: " << p->type() << "\n"
               << "length: " << p->length() << "\n";
        }
        static void finalize(packet p) {
            if (p.next(senf::nothrow))
                p->type() = senf::PacketRegistry<RegTag>::key(p.next());
            else
                p->type() = -1;
        }
        static key_t nextPacketKey(packet p) {
            return p->type();
        }
    };
    typedef BarPacketType::packet BarPacket;

    namespace reg {
        senf::PacketRegistry<RegTag>::RegistrationProxy<FooPacket> registerFoo(1u);
        senf::PacketRegistry<RegTag>::RegistrationProxy<BarPacket> registerBar(2u);
    }

    struct IntAnnotation {
        unsigned value;
    };
    
    struct LargeAnnotation {
        char value[32];
    };

    struct ComplexAnnotation : senf::ComplexAnnotation
    {
        std::string s;
        int i;
    };

    struct ComplexEmptyAnnotation : senf::ComplexAnnotation
    {};

    struct InvalidAnnotation
    {
        std::string value;
    };

}

BOOST_AUTO_UNIT_TEST(packet)
{
    senf::Packet packet (FooPacket::create());    
    BarPacket::createAfter(packet);

    BOOST_REQUIRE( packet );
    BOOST_CHECK( packet.next() );
    BOOST_CHECK( ! packet.next().next(senf::nothrow) );
    BOOST_CHECK( ! packet.prev(senf::nothrow) );
    BOOST_CHECK( packet.next().prev() == packet );
    BOOST_CHECK( packet.next() != packet );
    BOOST_CHECK_EQUAL( std::distance(packet.data().begin(), packet.next().data().begin()), 4 );
    BOOST_CHECK_EQUAL( std::distance(packet.data().begin(), packet.data().end()), 12 );
    BOOST_CHECK_EQUAL( std::distance(packet.next().data().begin(), packet.next().data().end()), 8 );
    BOOST_CHECK( packet.data().end() == packet.next().data().end() );
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
    
    packet.finalizeAll();
    BOOST_CHECK_EQUAL( packet.last().as<BarPacket>()->type(), 
                       BarPacket::Parser::type_t::value_type(-1) );
    packet.last().append(FooPacket::create());
    packet.finalizeThis();
    packet.finalizeTo<BarPacket>();
    packet.finalizeTo(packet.find<BarPacket>());
    packet.finalizeAll();
    BOOST_CHECK_EQUAL( packet.find<BarPacket>()->type(), 1u );

    BOOST_CHECK( packet.factory() == FooPacket::factory() );

    senf::PacketData::byte data[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x81, 0x82, 0x83 };

    BarPacket::createAfter(packet, data);
    BOOST_REQUIRE( packet.next() );
    BOOST_REQUIRE( packet.next().is<BarPacket>() );
    BOOST_CHECK( packet.last().is<FooPacket>() );
    BOOST_CHECK_EQUAL( packet.last().rfind<BarPacket>()->type(), 1u );
    BOOST_CHECK_EQUAL( packet.next().size(), 11u );
    BOOST_REQUIRE( packet.next().next() );
    BOOST_CHECK( packet.next().next().is<FooPacket>() );
    BOOST_CHECK( ! packet.next().next().next(senf::nothrow) );
    BOOST_CHECK_EQUAL( packet.next().next().data()[0], 0x81u );

    BOOST_CHECK( packet.first().find<FooPacket>() == packet );
    BOOST_CHECK( packet.last().rfind<BarPacket>() == packet.last().prev() );
    BOOST_CHECK( packet.find<FooPacket>() == packet );
    BOOST_CHECK( packet.last().rfind<FooPacket>() == packet.last() );
    BOOST_CHECK( packet.next<BarPacket>() == packet.next() );
    BOOST_CHECK( packet.last().prev().prev<FooPacket>() == packet );
    
    senf::DataPacket::createAfter(packet);
    BOOST_CHECK_THROW( packet.next().next().next().parseNextAs<BarPacket>(),
            senf::InvalidPacketChainException );
}

BOOST_AUTO_UNIT_TEST(concretePacket)
{
    senf::PacketData::byte data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

    BOOST_CHECK_EQUAL( FooPacket::create().size(), 4u );
    BOOST_CHECK_EQUAL( FooPacket::create(senf::noinit).size(), 0u );
    BOOST_CHECK_THROW( FooPacket::create(2u), senf::TruncatedPacketException );
    // No 'u' suffix here to check, that the disable_if works ...
    BOOST_CHECK_EQUAL( FooPacket::create(10).size(), 10u );
    BOOST_CHECK_EQUAL( FooPacket::create(2u,senf::noinit).size(), 2u );
    BOOST_CHECK_EQUAL( FooPacket::create(data).size(), 6u );

    senf::Packet packet (FooPacket::create());

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet).size(), 4u );
    BOOST_CHECK_EQUAL( packet.size(), 8u );

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,senf::noinit).size(), 0u );
    BOOST_CHECK_EQUAL( packet.size(), 4u );

    BOOST_CHECK_THROW( FooPacket::createAfter(packet,2u), senf::TruncatedPacketException );
    // No 'u' suffix here to check, that the disable_if works ...
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,10).size(), 10u );
    BOOST_CHECK_EQUAL( packet.size(), 14u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,2u,senf::noinit).size(), 2u );
    BOOST_CHECK_EQUAL( packet.size(), 6u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,data).size(), 6u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );
    
    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet).size(), 14u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );

    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet,senf::noinit).size(), 10u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );

    BOOST_CHECK( packet.clone() != packet );
    BOOST_CHECK_EQUAL( BarPacket::create()->reserved(), 0xA0A0u );
}

BOOST_AUTO_UNIT_TEST(packetAssign)
{
    BarPacket bar1 (BarPacket::create());
    BarPacket bar2 (BarPacket::create());

    bar2->type() << 0x2A2Bu;
    bar1.parser() << bar2;
    
    BOOST_CHECK_EQUAL( bar1->type(), 0x2A2Bu );
}

BOOST_AUTO_UNIT_TEST(packetAnnotation)
{
    senf::Packet packet (FooPacket::create());
    BarPacket::createAfter(packet);

    ComplexAnnotation & ca (packet.annotation<ComplexAnnotation>());
    ca.s = "dead beef";
    ca.i = 0x12345678;
    SENF_CHECK_NO_THROW( packet.annotation<IntAnnotation>().value = 0xDEADBEEF );

    senf::Packet p2 (packet.next());

    BOOST_CHECK_EQUAL( p2.annotation<IntAnnotation>().value, 0xDEADBEEFu );
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().s, "dead beef" );
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().i, 0x12345678 );

    BOOST_CHECK( senf::detail::AnnotationIndexer<IntAnnotation>::Small );
    BOOST_CHECK( ! senf::detail::AnnotationIndexer<LargeAnnotation>::Small );
    BOOST_CHECK( ! senf::detail::AnnotationIndexer<ComplexAnnotation>::Small );
    BOOST_CHECK( ! senf::detail::AnnotationIndexer<ComplexEmptyAnnotation>::Small );
}

#ifdef COMPILE_CHECK

COMPILE_FAIL(invalidAnnotation)
{
#if 0 // The traits check fails for user defined but trivial constructors so ...
#   ifdef BOOST_HAS_TYPE_TRAITS_INTRINSICS

    senf::Packet packet (FooPacket::create());
    (void) packet.annotation<InvalidAnnotation>();

#   else
#   endif
#endif

    invalid_annotation_check_disabled();

}

#endif

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
