//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief BitseParser unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

namespace {
    struct VoidPacket : public PacketTypeBase
    {};
}

SENF_AUTO_TEST_CASE(parse_bitset)
{
    typedef BitsetParser<7, LSB0Endianness> LSB0BitsetParser7;
    typedef BitsetParser<8, LSB0Endianness> LSB0BitsetParser8;
    typedef BitsetParser<9, LSB0Endianness> LSB0BitsetParser9;
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
        BitsetParser<2*8, LSB0Endianness> bitset (p->data().begin(), &p->data());

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

