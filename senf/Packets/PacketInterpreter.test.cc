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
    \brief PacketInterpreter unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {
        typedef senf::ConcretePacket<VoidPacket> packet;
        static optional_range nextPacketRange(packet p)
            { return range(p.data().begin(), p.data().end()); }
    };
}

SENF_AUTO_TEST_CASE(packetInterpreterBase)
{
    {
        senf::PacketInterpreter<VoidPacket>::ptr pi2 (senf::PacketInterpreter<VoidPacket>::create());
        senf::PacketInterpreter<VoidPacket>::ptr pi1 (senf::PacketInterpreter<VoidPacket>::createBefore(pi2));

        pi2->data().insert(pi2->data().begin(),0x02);
        BOOST_CHECK_EQUAL( pi1->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );

        senf::PacketInterpreter<VoidPacket>::ptr pi3 (pi2->parseNextAs<VoidPacket>());
        BOOST_REQUIRE( pi3 );
        BOOST_CHECK( pi2 == pi1->next() );
        BOOST_CHECK( pi3 == pi2->next() );
        BOOST_CHECK( ! pi3->next() );
        BOOST_CHECK( pi2 == pi3->prev() );
        BOOST_CHECK( pi1 == pi2->prev() );
        BOOST_CHECK( ! pi1->prev() );
        BOOST_CHECK( pi2->as<VoidPacket>() == pi2 );
        BOOST_CHECK( pi2->parseNextAs(pi2, senf::PacketInterpreter<VoidPacket>::factory(), pi2->nextPacketRange()) );
        BOOST_CHECK( pi2->typeId() == pi1->typeId() );

        pi1->data().insert(pi1->data().begin(),2,0x01u);
        BOOST_CHECK_EQUAL( pi1->data().size(), 3u );
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi3->data().size(), 1u );

        senf::PacketInterpreter<VoidPacket>::ptr pi2b (pi2->clone());
        BOOST_REQUIRE( pi2b->next() );
        BOOST_CHECK( ! pi2b->next()->next() );
        BOOST_CHECK( ! pi2b->prev() );

        pi2b->data().insert(pi2b->data().begin(),0x03u);
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi2b->data().size(), 2u );

        BOOST_CHECK_EQUAL( pi1->data().size(), unsigned(pi1->nextPacketRange()->size()) );
        pi1->append(pi2b);
        BOOST_CHECK_EQUAL( pi1->data().size(), 2u );
        BOOST_REQUIRE( pi1->next() );
        BOOST_REQUIRE( pi1->next()->next() );
        BOOST_CHECK( ! pi1->next()->next()->next() );
    }

}

namespace {
    struct OtherPacket
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<OtherPacket>
    {
        using senf::PacketTypeMixin<OtherPacket>::nextPacketRange;
        typedef senf::ConcretePacket<OtherPacket> packet;
        static size_type initSize()     { return 8u; }
        static size_type initHeadSize() { return 6u; }
        static void init(packet p)            { p.data()[0] = 0x01u; }
    };
}

SENF_AUTO_TEST_CASE(packetInterpreter)
{
    typedef senf::PacketInterpreterBase::size_type size_type;
    {
        BOOST_CHECK_THROW( senf::PacketInterpreter<OtherPacket>::create(size_type(4u)),
                           senf::TruncatedPacketException );
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(12u)));

        BOOST_CHECK_EQUAL( p->data().size(), 12u );
        BOOST_CHECK_EQUAL( std::distance(p->data().begin(),p->data().end()), 12 );
        BOOST_CHECK_EQUAL( p->data()[0], 0x01u );

        senf::PacketInterpreter<VoidPacket>::ptr p2
            (p->parseNextAs<VoidPacket>());
        BOOST_CHECK_EQUAL( p2->data().size(), 4u );
        BOOST_CHECK_EQUAL( std::distance(p->data().begin(),p2->data().begin()), 6 );

        senf::PacketInterpreter<OtherPacket>::ptr pc
            (p->clone());
        BOOST_CHECK_EQUAL( p->data().size(), 12u );
        BOOST_CHECK( p->next() );
        BOOST_CHECK( ! p->next()->next() );
    }

    {
        SENF_CHECK_NO_THROW(
            senf::PacketInterpreter<OtherPacket>::create(4u,senf::noinit));
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(senf::noinit));
        BOOST_CHECK_EQUAL( p->data().size(), 0u );
    }

    {
        senf::PacketInterpreter<VoidPacket>::byte data[] = { 0x01, 0x02, 0x03, 0x04 };
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create(data));

        BOOST_CHECK_EQUAL( p->data().size(), 4u );
        BOOST_CHECK_EQUAL( std::distance(p->data().begin(),p->data().end()), 4 );
        BOOST_CHECK( std::equal(p->data().begin(), p->data().end(), data) );
    }

    {
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(12u)));
        senf::PacketInterpreter<VoidPacket>::ptr p2
            (senf::PacketInterpreter<VoidPacket>::createAfter(p));
        BOOST_CHECK_EQUAL( p2->data().size(), 0u );
        BOOST_CHECK_EQUAL( p->data().size(), 8u );
        p2->data().insert(p2->data().begin(),0x01u);
        BOOST_CHECK_EQUAL( p->data()[6], 0x01u );
        BOOST_CHECK_EQUAL( p->data().size(), 9u );
    }

    {
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(12u)));
        senf::PacketInterpreter<OtherPacket>::ptr p2
            (senf::PacketInterpreter<OtherPacket>::createAfter(
                p,senf::noinit));
        BOOST_CHECK_EQUAL( p2->data().size(), 0u );
        BOOST_CHECK_EQUAL( p->data().size(), 8u );
    }

    {
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(12u)));
        senf::PacketInterpreter<OtherPacket>::ptr p2
            (senf::PacketInterpreter<OtherPacket>::createAfter(p, size_type(10u)));
        BOOST_CHECK_EQUAL( p2->data().size(), 10u );
        BOOST_CHECK_EQUAL( p->data().size(), 18u );
        BOOST_CHECK_EQUAL( p2->data()[0], 0x01u );
        BOOST_CHECK_THROW( senf::PacketInterpreter<OtherPacket>::createAfter(p, size_type(4u)),
                           senf::TruncatedPacketException );
        SENF_CHECK_NO_THROW( senf::PacketInterpreter<OtherPacket>::createAfter(
                                  p,4u,senf::noinit) );
    }

    {
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(10u)));
        senf::PacketInterpreter<VoidPacket>::byte data[] = { 0x01, 0x02, 0x03, 0x04 };
        senf::PacketInterpreter<OtherPacket>::ptr p2
            (senf::PacketInterpreter<OtherPacket>::createAfter(p,data));
        BOOST_CHECK_EQUAL( p2->data().size(), 4u );
        BOOST_CHECK_EQUAL( p->data().size(), 12u );
        BOOST_CHECK_EQUAL( p2->data()[1], 0x02u );
        BOOST_CHECK_EQUAL( p->data()[8], 0x03u );
    }

    {
        senf::PacketInterpreter<OtherPacket>::ptr p
            (senf::PacketInterpreter<OtherPacket>::create(size_type(10u)));
        senf::PacketInterpreter<OtherPacket>::ptr p2
            (senf::PacketInterpreter<OtherPacket>::createBefore(p));

        BOOST_CHECK_EQUAL( p2->data().size(), 18u );
        BOOST_CHECK_EQUAL( p->data().size(), 10u );

        senf::PacketInterpreter<OtherPacket>::ptr p3
            (senf::PacketInterpreter<OtherPacket>::createBefore(
                p,senf::noinit));

        BOOST_CHECK_EQUAL( p3->data().size(), 10u );

        senf::PacketInterpreter<OtherPacket>::ptr p4
            (senf::PacketInterpreter<OtherPacket>::createInsertBefore(p));

        BOOST_CHECK_EQUAL( p->data().size(), 10u );
        BOOST_CHECK_EQUAL( p4->data().size(), 18u );
        BOOST_CHECK_EQUAL( p3->data().size(), 18u );
    }

}

// fields() is tested in DefaultBundle/EthernetPacket.test.cc
// initSize() and initHeadSize() are already tested indirectly above

SENF_AUTO_TEST_CASE(packetInterpreter_factory)
{
    typedef senf::PacketInterpreterBase::size_type size_type;

    senf::PacketInterpreterBase::byte data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    senf::PacketInterpreterBase::factory_t factory (
        senf::PacketInterpreter<OtherPacket>::factory());

    BOOST_CHECK( ! senf::PacketInterpreterBase::no_factory() );
    BOOST_REQUIRE( factory );

    BOOST_CHECK_EQUAL( factory->create()->data().size(), 8u );
    BOOST_CHECK_EQUAL( factory->create(senf::noinit)->data().size(), 0u );
    BOOST_CHECK_EQUAL( factory->create(size_type(12u))->data().size(), 12u );
    BOOST_CHECK_EQUAL(
        factory->create(4u, senf::noinit)->data().size(), 4u );
    BOOST_CHECK_EQUAL( factory->create(data)->data().size(), 6u );

    {
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create());

        BOOST_CHECK_EQUAL( p->data().size(), 0u);
        BOOST_CHECK( factory->createAfter(p) );
        BOOST_REQUIRE( p->next() );
        BOOST_CHECK( ! p->next()->next() );

        BOOST_CHECK_EQUAL( factory->createAfter(p)->data().size(), 8u );
        BOOST_CHECK_EQUAL( p->data().size(), 8u );
        BOOST_CHECK( ! p->next()->next() );

        BOOST_CHECK_EQUAL(
            factory->createAfter(p, senf::noinit)->data().size(), 0u );
        BOOST_CHECK_EQUAL( p->data().size(), 0u );
        BOOST_CHECK( ! p->next()->next() );

        BOOST_CHECK_EQUAL( factory->createAfter(p, size_type(12u))->data().size(), 12u );
        BOOST_CHECK_EQUAL( p->data().size(), 12u );
        BOOST_CHECK( ! p->next()->next() );

        BOOST_CHECK_EQUAL(
            factory->createAfter(p,4u, senf::noinit)->data().size(), 4u );
        BOOST_CHECK_EQUAL( p->data().size(), 4u );
        BOOST_CHECK( ! p->next()->next() );

        BOOST_CHECK_EQUAL( factory->createAfter(p, data)->data().size(), 6u );
        BOOST_CHECK_EQUAL( p->data().size(), 6u );
        BOOST_CHECK( ! p->next()->next() );
    }

    {
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create(size_type(4u)));

        BOOST_CHECK_EQUAL( factory->createBefore(p)->data().size(), 12u );
        BOOST_REQUIRE( p->prev() );
        BOOST_CHECK( ! p->prev()->prev() );
        BOOST_CHECK_EQUAL( p->prev()->data().size(), 12u );

        BOOST_CHECK_EQUAL( factory->createBefore(p,senf::noinit)->data().size(), 4u );
        BOOST_REQUIRE( p->prev() );
        BOOST_CHECK( ! p->prev()->prev() );
        BOOST_CHECK_EQUAL( p->prev()->data().size(), 4u );

        BOOST_CHECK_EQUAL( factory->createInsertBefore(p,senf::noinit)->data().size(), 4u );
        BOOST_REQUIRE( p->prev() );
        BOOST_REQUIRE( p->prev()->prev() );
        BOOST_CHECK( ! p->prev()->prev()->prev() );
    }

    {
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create(size_type(12u)));

        senf::PacketInterpreterBase::ptr p2 (p->parseNextAs(p, factory, p->nextPacketRange()));
        BOOST_CHECK_EQUAL( unsigned(boost::size(*p2->nextPacketRange())), 4u );
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
