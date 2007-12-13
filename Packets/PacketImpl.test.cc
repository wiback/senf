// $Id$
//
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
    \brief PacketImpl.test unit tests */

//#include "PacketImpl.test.hh"
//#include "PacketImpl.test.ih"

// Custom includes
#include "Packets.hh"
#include "main.test.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};
}

// PacketImpl cannot be tested without relying on PacketInterpreterBase.  However these unit-tests
// only validate PacketInterpreterBase as far as to ensure that a failure of one test is not due to
// an error in PacketInterpreterbase

BOOST_AUTO_UNIT_TEST(packetImpl_mem)
{
    senf::detail::PacketImpl * p (new senf::detail::PacketImpl());
    BOOST_CHECK_EQUAL(p->refcount(), 0);
    p->add_ref();
    BOOST_CHECK_EQUAL(p->refcount(), 1);
#ifndef NDEBUG
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 1u);
#endif
    // From now on, the object should stay alive since I manually incremented the
    // refcount ..


    p->add_ref(2);
    BOOST_CHECK_EQUAL(p->refcount(), 3);
    p->release(2);
    BOOST_CHECK_EQUAL(p->refcount(), 1);

    {
        senf::PacketInterpreterBase::ptr pi (
            senf::detail::packet::test::TestDriver::create<VoidPacket>(
                p,p->begin(),p->end(), senf::PacketInterpreterBase::Append));
        // Hmm ... this check works as long as sizeof(PacketInterpreterBase> !=
        // sizeof(PacketImpl) ... !!
#ifndef NDEBUG
        BOOST_CHECK_EQUAL(
            senf::pool_alloc_mixin< senf::PacketInterpreter<VoidPacket> >::allocCounter(), 1u);
#endif
        senf::PacketInterpreterBase::ptr pi2 (pi);
        BOOST_CHECK_EQUAL(p->refcount(), 3);
    }
    BOOST_CHECK_EQUAL(p->refcount(),1);

    {
        senf::PacketInterpreterBase::ptr pi (p->first());
        BOOST_CHECK_EQUAL(p->refcount(),2);
        p->truncateInterpreters(pi.get());
        BOOST_CHECK_EQUAL(p->refcount(),1);
    }
#ifndef NDEBUG
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::PacketInterpreterBase>::allocCounter(), 0u);
#endif
    BOOST_CHECK_EQUAL(p->refcount(),1);


    // The refcount must be one here (from incrementing the refcount above)
    // Therefore we can safely delete the object.
    BOOST_CHECK_EQUAL(p->refcount(), 1);
    p->release();
#ifndef NDEBUG
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
#endif
}

BOOST_AUTO_UNIT_TEST(packetImpl_data)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create());
    senf::detail::PacketImpl * p (senf::detail::packet::test::TestDriver::impl(pi));

    senf::detail::PacketImpl::byte data[] = 
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

    p->insert(&pi->data(),p->begin(),data, data+sizeof(data));
    BOOST_CHECK_EQUAL(p->size(), 8u);
    BOOST_CHECK_EQUAL(p->begin()[0], 0x00u);
    BOOST_CHECK_EQUAL(p->begin()[7], 0x07u);
    BOOST_CHECK_EQUAL(pi->data().size(), p->size());
    
    p->insert(&pi->data(),p->begin()+2,0xf0u);
    BOOST_CHECK_EQUAL(p->size(),9u);
    BOOST_CHECK_EQUAL(p->begin()[8], 0x07u);
    BOOST_CHECK_EQUAL(p->begin()[2], 0xf0u);
    BOOST_CHECK_EQUAL(pi->data().size(), p->size());

    p->insert(&pi->data(),p->begin()+9,8,0xffu);
    BOOST_CHECK_EQUAL(p->size(),17u);
    BOOST_CHECK_EQUAL(p->begin()[16], 0xffu);
    BOOST_CHECK_EQUAL(p->begin()[8], 0x07u);
    BOOST_CHECK_EQUAL(pi->data().size(), p->size());

    p->erase(&pi->data(),p->begin());
    BOOST_CHECK_EQUAL(p->size(),16u);
    BOOST_CHECK_EQUAL(p->begin()[0], 0x01u);
    BOOST_CHECK_EQUAL(pi->data().size(), p->size());
    
    p->erase(&pi->data(),p->begin()+2, p->begin()+7);
    BOOST_CHECK_EQUAL(p->size(),11u);
    BOOST_CHECK_EQUAL(p->begin()[2], 0x07u);
    BOOST_CHECK_EQUAL(p->begin()[3], 0xffu);
    BOOST_CHECK_EQUAL(pi->data().size(), p->size());

    BOOST_REQUIRE_EQUAL(pi->data().size(), p->size());
    BOOST_REQUIRE(pi->data().begin() == p->begin());

    p->clear(&pi->data());
    BOOST_CHECK_EQUAL(p->size(), 0u);
    BOOST_CHECK_EQUAL(pi->data().size(), 0u);
    BOOST_CHECK(pi->data().begin() == p->begin());
}

BOOST_AUTO_UNIT_TEST(packetImpl_interpreters)
{
    senf::detail::PacketImpl * p (new senf::detail::PacketImpl());
    p->add_ref();

    {
        senf::PacketInterpreterBase::ptr pi2 (
            senf::detail::packet::test::TestDriver::create<VoidPacket>(
                p,p->begin(),p->end(),senf::PacketInterpreterBase::Append));
        senf::PacketInterpreterBase::ptr pi3 (
            senf::detail::packet::test::TestDriver::create<VoidPacket>(
                p,p->end(),p->end(),senf::PacketInterpreterBase::Append));
        senf::PacketInterpreterBase::ptr pi1 (
            senf::detail::packet::test::TestDriver::create<VoidPacket>(
                p,p->begin(),p->end(),senf::PacketInterpreterBase::Prepend));

        BOOST_CHECK_EQUAL(p->first(), pi1.get());
        BOOST_CHECK_EQUAL(p->next(p->first()), pi2.get());
        BOOST_CHECK_EQUAL(p->next(p->next(p->first())), pi3.get());
        BOOST_CHECK( !p->next(p->next(p->next(p->first()))) );

        BOOST_CHECK_EQUAL(p->last(), pi3.get());
        BOOST_CHECK_EQUAL(p->prev(p->last()), pi2.get());
        BOOST_CHECK_EQUAL(p->prev(p->prev(p->last())), pi1.get());
        BOOST_CHECK( !p->prev(p->prev(p->prev(p->last()))) );

        p->insert(&pi2->data(),p->begin(),10,0x00u);
        BOOST_CHECK_EQUAL(pi1->data().size(), 10u);
        BOOST_CHECK_EQUAL(pi2->data().size(), 10u);
        BOOST_CHECK_EQUAL(pi3->data().size(), 0u);
        BOOST_CHECK( pi1->data().begin() == p->begin() );
        BOOST_CHECK( pi2->data().begin() == p->begin() );
        BOOST_CHECK( pi3->data().begin() == p->end() );
        
        p->insert(&pi3->data(),p->end(), 0x00u);
        BOOST_CHECK_EQUAL(pi1->data().size(), 11u);
        BOOST_CHECK_EQUAL(pi2->data().size(), 11u);
        BOOST_CHECK_EQUAL(pi3->data().size(), 1u);
        
        p->insert(&pi1->data(),p->end(), 2, 0x00u);
        BOOST_CHECK_EQUAL(pi1->data().size(), 13u);
        BOOST_CHECK_EQUAL(pi2->data().size(), 11u);
        BOOST_CHECK_EQUAL(pi3->data().size(), 1u);
        BOOST_CHECK( pi1->data().end() == p->begin()+13u );
        BOOST_CHECK( pi2->data().end() == p->begin()+11u );
        BOOST_CHECK( pi3->data().end() == p->begin()+11u );

        p->clear(&pi2->data());
        BOOST_CHECK_EQUAL(pi1->data().size(), 2u);
        BOOST_CHECK( ! p->next(p->next(p->first())) );
    }

    BOOST_CHECK_EQUAL(p->refcount(), 1);
    p->release();
#ifndef NDEBUG
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
#endif
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
