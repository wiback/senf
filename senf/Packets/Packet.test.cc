// $Id$
//
// Copyright (C) 2007
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
    \brief Packet unit tests */

//#include "Packet.test.hh"
//#include "Packet.test.ih"

// Custom includes
#include <sstream>
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include <boost/test/execution_monitor.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
               << senf::fieldName("type") << p->type() << "\n"
               << senf::fieldName("length") << p->length() << "\n";
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
        senf::PacketRegistry<RegTag>::ScopedRegistrationProxy<FooPacket> registerFoo(1u);
        senf::PacketRegistry<RegTag>::ScopedRegistrationProxy<BarPacket> registerBar(2u);
    }

    struct IntAnnotation {
        boost::uint32_t value;
    };

    std::ostream & operator<<(std::ostream & os, IntAnnotation const & v)
    { os << v.value; return os; }

    struct LargeAnnotation {
        char value[32];
    };

    std::ostream & operator<<(std::ostream & os, LargeAnnotation const & v)
    { os << v.value; return os; }

    struct ComplexAnnotation : senf::ComplexAnnotation
    {
        ComplexAnnotation() : s("empty"), i(-1) {}
        std::string s;
        boost::int32_t i;
        // padding so the size does not depend on the platform ...
        struct _ {std::string s;boost::int32_t i;};
        char __ [32-sizeof(_)];
    };

    std::ostream & operator<<(std::ostream & os, ComplexAnnotation const & v)
    { os << "('" << v.s << "' " << v.i << ')'; return os; }

    struct ComplexEmptyAnnotation : senf::ComplexAnnotation
    {};

    std::ostream & operator<<(std::ostream & os, ComplexEmptyAnnotation const & v)
    { os << "(empty)"; return os; }

    bool compareIgnoreAddresses(std::string a, std::string b)
    {
        for (std::string::size_type i (a.find("0x"));
             i != std::string::npos; i = a.find("0x", i+2)) {
            while (std::isxdigit(a[i+2]) or a[i+2] == '?')
                a.erase(i+2,1);
        }
        for (std::string::size_type i (b.find("0x"));
             i != std::string::npos; i = b.find("0x", i+2)) {
            while (std::isxdigit(b[i+2]) or b[i+2] == '?')
                b.erase(i+2,1);
        }
        return a == b;
    }

    std::string memDebugInfo(senf::Packet const & packet)
    {
        std::stringstream ss;
        packet.memDebug(ss);
        return ss.str();
    }

}

namespace {

    template <class T, unsigned N>
    std::string str(T const (& data) [N])
    { return std::string(static_cast<char const *>(static_cast<void const *>(data)),
                         static_cast<char const *>(static_cast<void const *>(data + N))); }

    template <class T>
    std::string str(T const & container)
    { return std::string(container.begin(), container.end()); }

    template <unsigned N>
    std::string strc(char const (& data) [N])
    { return std::string(static_cast<char const *>(static_cast<void const *>(data)),
                         static_cast<char const *>(static_cast<void const *>(data + N - 1))); }

}

SENF_AUTO_UNIT_TEST(packet)
{
    BOOST_CHECK(! senf::Packet().is<BarPacket>() );
    senf::Packet packet (FooPacket::create());
    BarPacket::createAfter(packet);

    BOOST_CHECK_THROW( senf::Packet().as<BarPacket>(), senf::WrapException<std::bad_cast> );
    BOOST_CHECK_THROW( packet.as<BarPacket>(), senf::WrapException<std::bad_cast> );

    BOOST_REQUIRE( packet );
    BOOST_CHECK( packet.next() );
    BOOST_CHECK( ! packet.next().next(senf::nothrow) );
    BOOST_CHECK( ! packet.next().next(senf::nothrow).is<BarPacket>() );
    BOOST_CHECK( ! packet.prev(senf::nothrow) );
    BOOST_CHECK( packet.next().prev() == packet );
    SENF_CHECK_NOT_EQUAL( packet.next(), packet );
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

    BOOST_CHECK( ! packet.is_shared() );
    {
        senf::Packet p2 (packet);
        BOOST_CHECK( packet.is_shared() );
        BOOST_CHECK( p2.is_shared() );
    }
    BOOST_CHECK( ! packet.is_shared() );

    senf::Packet p2 (packet.next());
    BOOST_CHECK_PREDICATE( compareIgnoreAddresses, (memDebugInfo(packet))(
                               "PacketImpl @0x????????" "-0x???????? refcount=2"
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
                               " preallocHigh=2"
#endif
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
                               " preallocHeapcount=0"
#endif
                               "\n"
#ifndef SENF_PACKET_ALTERNATIVE_PREALLOC
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
#endif
                               "  handle @0x????????\n"
                               "  interpreter @0x???????? refcount=1\n"
                               "  interpreter @0x???????? refcount=1\n") );
    BOOST_CHECK( p2 );
    BOOST_CHECK( packet.is_shared() );
    BOOST_CHECK( p2.is_shared() );
    packet.parseNextAs<FooPacket>();
    BOOST_CHECK_PREDICATE( compareIgnoreAddresses, (memDebugInfo(packet))(
                               "PacketImpl @0x????????" "-0x???????? refcount=2"
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
                               " preallocHigh=3"
#endif
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
                               " preallocHeapcount=0"
#endif
                               "\n"
                               "  unlinked @0x???????? refcount=1\n"
#ifndef SENF_PACKET_ALTERNATIVE_PREALLOC
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
#endif
                               "  handle @0x????????\n"
                               "  interpreter @0x???????? refcount=1\n"
                               "  interpreter @0x???????? refcount=0\n") );
    BOOST_CHECK_EQUAL( packet.size(), 12u );
    BOOST_CHECK_EQUAL( packet.next().size(), 8u );
    BOOST_CHECK( packet.next().is<FooPacket>() );
    BOOST_CHECK( packet.next().as<FooPacket>() );
    BOOST_CHECK_PREDICATE( compareIgnoreAddresses, (memDebugInfo(packet))(
                               "PacketImpl @0x????????" "-0x???????? refcount=2"
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
                               " preallocHigh=3"
#endif
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
                               " preallocHeapcount=0"
#endif
                               "\n"
                               "  unlinked @0x???????? refcount=1\n"
#ifndef SENF_PACKET_ALTERNATIVE_PREALLOC
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
#endif
                               "  handle @0x????????\n"
                               "  interpreter @0x???????? refcount=1\n"
                               "  interpreter @0x???????? refcount=0\n") );

    p2 = packet.next().clone();
    BOOST_CHECK_PREDICATE( compareIgnoreAddresses, (memDebugInfo(packet))(
                               "PacketImpl @0x????????" "-0x???????? refcount=1"
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
                               " preallocHigh=3"
#endif
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
                               " preallocHeapcount=0"
#endif
                               "\n"
                               "  free @0x????????\n"
#ifndef SENF_PACKET_ALTERNATIVE_PREALLOC
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
                               "  free @0x????????\n"
#endif
                               "  handle @0x????????\n"
                               "  interpreter @0x???????? refcount=1\n"
                               "  interpreter @0x???????? refcount=0\n") );
    BOOST_CHECK( ! packet.is_shared() );
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
    BOOST_CHECK_EQUAL( s.str(),
                       "Annotations:\n"
                       "  (anonymous namespace)::IntAnnotation : 0\n"
                       "BarPacket:\n"
                       "  type                    : 0\n"
                       "  length                  : 0\n" );

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

    SENF_CHECK_NO_THROW( BarPacket::create(senf::noinit).dump(s));
}

SENF_AUTO_UNIT_TEST(concretePacket)
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

    BOOST_CHECK_EQUAL( FooPacket::createInsertBefore(packet).size(), 14u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );
    BOOST_REQUIRE( packet.prev() );
    BOOST_CHECK_EQUAL( packet.prev().size(), 14u );
    BOOST_REQUIRE( packet.prev().prev() );
    BOOST_CHECK_EQUAL( packet.prev().prev().size(), 14u );

    BOOST_CHECK_EQUAL( FooPacket::createInsertBefore(packet,senf::noinit).size(), 10u );
    BOOST_CHECK_EQUAL( packet.size(), 10u );
    BOOST_REQUIRE_NO_THROW( packet.prev().prev().prev() );
    BOOST_CHECK_THROW( packet.prev().prev().prev().prev(), senf::InvalidPacketChainException );
    BOOST_CHECK_EQUAL( packet.prev().size(), 10u );
    BOOST_CHECK_EQUAL( packet.prev().prev().size(), 14u );
    BOOST_CHECK_EQUAL( packet.prev().prev().prev().size(), 14u );

    SENF_CHECK_NOT_EQUAL( packet.clone(), packet );
    BOOST_CHECK_EQUAL( BarPacket::create()->reserved(), 0xA0A0u );
}

SENF_AUTO_UNIT_TEST(packetExternalMemory)
{
    senf::Packet::byte storage[] = "\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x11";
    BarPacket bar (BarPacket::create(storage, 8, 12, 4));

    BOOST_CHECK_EQUAL( bar->type(), 1u );
    BOOST_CHECK_EQUAL( bar->length(), 2 );
    BOOST_CHECK_EQUAL( bar->reserved(), 0x1100u );
#ifndef SENF_PACKET_STD_CONTAINER
    BOOST_CHECK( bar.data().usingExternalMemory() );
#endif

    bar->length() = 4;
#ifndef SENF_PACKET_STD_CONTAINER
    BOOST_CHECK_EQUAL( storage[9], 4u );
#endif

    FooPacket foo (FooPacket::createBefore(bar));

    foo->data()[0] = 0xaau;
#ifndef SENF_PACKET_STD_CONTAINER
    BOOST_CHECK_EQUAL( storage[0], 0xaau);
#endif

    foo.data().releaseExternalMemory();
    BOOST_CHECK( ! foo.data().usingExternalMemory() );
    BOOST_CHECK( ! bar.data().usingExternalMemory() );
    bar->length() = 6;
#ifndef SENF_PACKET_STD_CONTAINER
    BOOST_CHECK_EQUAL( storage[9], 4u );
#endif
    BOOST_CHECK_EQUAL( foo->data()[9], 6u );
}

SENF_AUTO_UNIT_TEST(packetAssign)
{
    BarPacket bar1 (BarPacket::create());
    BarPacket bar2 (BarPacket::create());

    bar2->type() << 0x2A2Bu;
    bar1.parser() << bar2;

    BOOST_CHECK_EQUAL( bar1->type(), 0x2A2Bu );
}

#ifdef SENF_CXX11_ENABLED

void movePacket(senf::Packet packet)
{
    BOOST_CHECK(! packet.is_shared());
}

SENF_AUTO_UNIT_TEST(packet_move)
{
    senf::Packet packet (FooPacket::create());
    movePacket(std::move(packet));
    BOOST_CHECK(! packet);
}

#endif

SENF_AUTO_UNIT_TEST(packetAnnotation)
{
    typedef senf::detail::AnnotationRegistry Reg;

    senf::Packet packet (FooPacket::create());
    BarPacket::createAfter(packet);

#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    ComplexAnnotation & ca (packet.annotation<ComplexAnnotation>());

    BOOST_CHECK_EQUAL( ca.s, "empty" );
    BOOST_CHECK_EQUAL( ca.i, -1 );

    ca.s = "dead beef";
    ca.i = 0x12345678;
#endif
    SENF_CHECK_NO_THROW( packet.annotation<IntAnnotation>().value = 0xDEADBEEF );

    senf::Packet p2 (packet.next());

    BOOST_CHECK_EQUAL( p2.annotation<IntAnnotation>().value, 0xDEADBEEFu );
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().s, "dead beef" );
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().i, 0x12345678 );
#endif

    senf::Packet pClone (packet.clone());

    p2.clearAnnotations();
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().s, "empty" );
    BOOST_CHECK_EQUAL( p2.annotation<ComplexAnnotation>().i, -1 );
#endif
    BOOST_CHECK_EQUAL( p2.annotation<IntAnnotation>().value, 0 );

    BOOST_CHECK_EQUAL( pClone.annotation<IntAnnotation>().value, 0xDEADBEEFu );
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    BOOST_CHECK_EQUAL( pClone.annotation<ComplexAnnotation>().s, "dead beef" );
    BOOST_CHECK_EQUAL( pClone.annotation<ComplexAnnotation>().i, 0x12345678 );
#endif

    BOOST_CHECK( Reg::lookup<IntAnnotation>() >= 0 );
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    BOOST_CHECK( Reg::lookup<LargeAnnotation>() < 0 );
    BOOST_CHECK( Reg::lookup<ComplexAnnotation>() < 0 );
    BOOST_CHECK( Reg::lookup<ComplexEmptyAnnotation>() < 0 );
#endif

    std::stringstream ss;
    senf::dumpPacketAnnotationRegistry(ss);
    BOOST_CHECK_EQUAL(
        ss.str(),
        "SENF_PACKET_ANNOTATION_SLOTS = 8\n"
        "SENF_PACKET_ANNOTATION_SLOTSIZE = 16\n"
#ifdef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
        "SENF_PACKET_NO_COMPLEX_ANNOTATIONS defined\n"
#endif
        "TYPE                                                      FAST  COMPLEX   SIZE\n"
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
        "(anonymous namespace)::ComplexAnnotation                  no    yes         32\n"
        "(anonymous namespace)::ComplexEmptyAnnotation             no    yes          1\n"
#endif
        "(anonymous namespace)::IntAnnotation                      yes   no           4\n"
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
        "(anonymous namespace)::LargeAnnotation                    no    no          32\n"
#endif
        );
}

#ifdef COMPILE_CHECK

namespace {
    struct InvalidAnnotation
    {
        std::string value;
    };

    std::ostream & operator<<(std::ostream & os, InvalidAnnotation const & v)
    { os << v.value; return os; }
}

COMPILE_FAIL(invalidAnnotation)
{
#if 0 // The traits check fails for user defined but trivial constructors so ...
#   ifdef BOOST_HAS_TYPE_TRAITS_INTRINSICS

    senf::Packet packet (FooPacket::create());
    senf::IGNORE( packet.annotation<InvalidAnnotation>() );

#   else
#   endif
#endif

    invalid_annotation_check_disabled();

}

#endif

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
