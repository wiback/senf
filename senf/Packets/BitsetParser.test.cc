// $Id$
//
// Copyright (C) 2013
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
    \brief BitseParser unit tests */

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

namespace {
    struct VoidPacket : public PacketTypeBase
    {};
}

SENF_AUTO_UNIT_TEST(parse_bitset)
{
    typedef BitsetParser<7,LSB0Endianness> LSB0BitsetParser7;
    typedef BitsetParser<8,LSB0Endianness> LSB0BitsetParser8;
    typedef BitsetParser<9,LSB0Endianness> LSB0BitsetParser9;
    BOOST_CHECK_EQUAL( LSB0BitsetParser7::fixed_bytes+0, 1);
    BOOST_CHECK_EQUAL( LSB0BitsetParser8::fixed_bytes+0, 1);
    BOOST_CHECK_EQUAL( LSB0BitsetParser9::fixed_bytes+0, 2);

    PacketInterpreterBase::byte data[] = {
            0x9a,  // 10011010
            0x8e   // 10001110
    };
    bool bits[] = { 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0 };

    {   // least significant bit first
        PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));
        BitsetParser<2*8,LSB0Endianness> bitset (p->data().begin(), &p->data());

        for (std::size_t i(0); i < bitset.size(); i++) {
            BOOST_CHECK_EQUAL( bitset.test(i), bits[15-i] );
            BOOST_CHECK_EQUAL( bitset[i], bits[15-i] );
        }
        bitset.flip();
        for (std::size_t i(0); i < bitset.size(); i++)
            BOOST_CHECK_EQUAL( bitset.test(i), ! bits[15-i] );
        bitset.flip();

        BOOST_CHECK_EQUAL( bitset.count(), 8u );
        BOOST_CHECK(   bitset.any() );
        BOOST_CHECK( ! bitset.none() );
        BOOST_CHECK( ! bitset.all() );

        bitset.set(0);
        BOOST_CHECK(   bitset[0] );
        bitset.reset(0);
        BOOST_CHECK( ! bitset[0] );
        bitset.flip(0);
        BOOST_CHECK(   bitset[0] );

        BOOST_CHECK_THROW( bitset.set(16),  senf::WrapException<std::out_of_range>);
        BOOST_CHECK_THROW( bitset.flip(16), senf::WrapException<std::out_of_range>);
        BOOST_CHECK_THROW( bitset.test(16), senf::WrapException<std::out_of_range>);

        bitset[0] = false;
        BOOST_CHECK( ! bitset[0] );
        bitset[0] = bitset[15];
        BOOST_CHECK(   bitset[0] );
        bitset[0].flip();
        BOOST_CHECK( ! bitset[0] );

        std::bitset<16> value (bitset.value());
        BOOST_CHECK_EQUAL( value.to_string(), "1001101010001110");
        value.flip();
        bitset.value( value);
        for (std::size_t i(0); i < bitset.size(); i++)
            BOOST_CHECK_EQUAL( bitset.test(i), ! bits[15-i] );

        bitset.reset();
        BOOST_CHECK( bitset.none() );
        bitset.set();
        BOOST_CHECK( bitset.all() );
    }
    {   // most significant bit first
        PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));
        BitsetParser<2*8,MSB0Endianness> bitset (p->data().begin(), &p->data());

        for (std::size_t i(0); i < bitset.size(); i++) {
            BOOST_CHECK_EQUAL( bitset.test(i), bits[i] );
            BOOST_CHECK_EQUAL( bitset[i], bits[i] );
        }
        bitset.flip();
        for (std::size_t i(0); i < bitset.size(); i++)
            BOOST_CHECK_EQUAL( bitset.test(i), ! bits[i] );
        bitset.flip();
    }

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_

