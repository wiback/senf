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

//#include "IntParser.test.hh"
//#include "IntParser.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

namespace {
    struct VoidPacket : public PacketTypeBase
    {};
}

BOOST_AUTO_UNIT_TEST(parseInt_fixedSizes)
{
    PacketInterpreterBase::byte data[] = { 0x8e, 0x2f, 0x57, 0x12, 0xd1 };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));

    BOOST_CHECK_EQUAL(Int8Parser(p->data().begin(),&p->data()).value(), -114);
    BOOST_CHECK_EQUAL(Int8Parser(p->data().begin()+1,&p->data()).value(), 47);
    BOOST_CHECK_EQUAL(UInt8Parser(p->data().begin(),&p->data()).value(), 142u);

    BOOST_CHECK_EQUAL(Int16Parser(p->data().begin(),&p->data()).value(), -29137);
    BOOST_CHECK_EQUAL(Int16Parser(p->data().begin()+1,&p->data()).value(), 12119);
    BOOST_CHECK_EQUAL(UInt16Parser(p->data().begin(),&p->data()).value(), 36399u);

    BOOST_CHECK_EQUAL(Int24Parser(p->data().begin(),&p->data()).value(), -7458985);
    BOOST_CHECK_EQUAL(Int24Parser(p->data().begin()+1,&p->data()).value(), 3102482);
    BOOST_CHECK_EQUAL(UInt24Parser(p->data().begin(),&p->data()).value(), 9318231u);

    BOOST_CHECK_EQUAL(Int32Parser(p->data().begin(),&p->data()).value(), -1909500142);
    BOOST_CHECK_EQUAL(Int32Parser(p->data().begin()+1,&p->data()).value(), 794235601);
    BOOST_CHECK_EQUAL(UInt32Parser(p->data().begin(),&p->data()).value(), 2385467154u);
}

BOOST_AUTO_UNIT_TEST(parseInt_bits)
{
    //                       0         1         2         3         4
    //                       012345678901234567890123456789012345678901234567
    //                       --------        --------        --------
    //                       011000111101011101011010001100011010010001000110
    PacketInterpreterBase::byte data[] = { 0x63,   0xd7,   0x5a,   0x31,   0xa4,   0x46 };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));

    // 1 byte
    BOOST_CHECK_EQUAL((UIntFieldParser<2,7>(p->data().begin(),&p->data()).value()), 17u);
    BOOST_CHECK_EQUAL((IntFieldParser<2,7>(p->data().begin(),&p->data()).value()), -15);
    BOOST_CHECK_EQUAL((UIntFieldParser<3,7>(p->data().begin(),&p->data()).value()), 1u);
    BOOST_CHECK_EQUAL((IntFieldParser<3,7>(p->data().begin(),&p->data()).value()), 1);
    BOOST_CHECK_EQUAL((UIntFieldParser<0,8>(p->data().begin(),&p->data()).value()), 99u);

    // 2 byte
    BOOST_CHECK_EQUAL((UIntFieldParser<5,12>(p->data().begin(),&p->data()).value()), 61u);
    BOOST_CHECK_EQUAL((UIntFieldParser<0,12>(p->data().begin(),&p->data()).value()), 1597u);
    BOOST_CHECK_EQUAL((UIntFieldParser<8,13>(p->data().begin(),&p->data()).value()), 26u);
    BOOST_CHECK_EQUAL((UIntFieldParser<8,16>(p->data().begin(),&p->data()).value()), 215u);
    BOOST_CHECK_EQUAL((UIntFieldParser<0,16>(p->data().begin(),&p->data()).value()), 25559u);

    // 3 byte
    BOOST_CHECK_EQUAL((UIntFieldParser<6,20>(p->data().begin(),&p->data()).value()), 15733u);
    BOOST_CHECK_EQUAL((IntFieldParser<6,20>(p->data().begin(),&p->data()).value()), -651);
    BOOST_CHECK_EQUAL((UIntFieldParser<13,22>(p->data().begin(),&p->data()).value()), 470u);

    // 4 byte
    BOOST_CHECK_EQUAL((UIntFieldParser<3,28>(p->data().begin(),&p->data()).value()), 4027811u);
    BOOST_CHECK_EQUAL((UIntFieldParser<13,38>(p->data().begin(),&p->data()).value()), 30837865u);
    BOOST_CHECK_EQUAL((UIntFieldParser<8,40>(p->data().begin(),&p->data()).value()), 3613012388u);
    BOOST_CHECK_EQUAL((IntFieldParser<8,40>(p->data().begin(),&p->data()).value()), -681954908);

    // 5 byte
    BOOST_CHECK_EQUAL((UIntFieldParser<3,34>(p->data().begin(),&p->data()).value()), 257779910u);
    BOOST_CHECK_EQUAL((IntFieldParser<13,41>(p->data().begin(),&p->data()).value()), -21732536);

    // single bit
    BOOST_CHECK_EQUAL((FlagParser<32>(p->data().begin(),&p->data()).value()), true);
    BOOST_CHECK_EQUAL((FlagParser<12>(p->data().begin(),&p->data()).value()), false);
}

BOOST_AUTO_UNIT_TEST(parseInt_assign)
{
    PacketInterpreterBase::byte data[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));

    Int8Parser(p->data().begin(),&p->data()).value(0x2f);
    BOOST_CHECK_EQUAL( p->data()[0], 0x2f );

    Int16Parser(p->data().begin(),&p->data()).value(0xa341);
    BOOST_CHECK_EQUAL( p->data()[0], 0xa3 );
    BOOST_CHECK_EQUAL( p->data()[1], 0x41 );

    Int24Parser(p->data().begin(),&p->data()).value(0x234567);
    BOOST_CHECK_EQUAL( p->data()[0], 0x23 );
    BOOST_CHECK_EQUAL( p->data()[1], 0x45 );
    BOOST_CHECK_EQUAL( p->data()[2], 0x67 );

    Int32Parser(p->data().begin(),&p->data()).value(0xfedcba98);
    BOOST_CHECK_EQUAL( p->data()[0], 0xfe );
    BOOST_CHECK_EQUAL( p->data()[1], 0xdc );
    BOOST_CHECK_EQUAL( p->data()[2], 0xba );
    BOOST_CHECK_EQUAL( p->data()[3], 0x98 );

    IntFieldParser<2,6>(p->data().begin(),&p->data()).value(0x3);
    BOOST_CHECK_EQUAL( p->data()[0], 0xce );
    BOOST_CHECK_EQUAL( p->data()[1], 0xdc );

    IntFieldParser<6,9>(p->data().begin(),&p->data()).value(0x2);
    BOOST_CHECK_EQUAL( p->data()[0], 0xcd );
    BOOST_CHECK_EQUAL( p->data()[1], 0x5c );
    BOOST_CHECK_EQUAL( p->data()[2], 0xba );

    IntFieldParser<2,21>(p->data().begin(),&p->data()).value(0x13d75);
    BOOST_CHECK_EQUAL( p->data()[0], 0xc9 );
    BOOST_CHECK_EQUAL( p->data()[1], 0xeb );
    BOOST_CHECK_EQUAL( p->data()[2], 0xaa );
    BOOST_CHECK_EQUAL( p->data()[3], 0x98 );

    UIntFieldParser<4,34>(p->data().begin(),&p->data()).value(0x268ad497u);
    BOOST_CHECK_EQUAL( (UIntFieldParser<4,34>(p->data().begin(),&p->data()).value()), 0x268ad497u );
}

BOOST_AUTO_UNIT_TEST(parseInt_operators)
{
    PacketInterpreterBase::byte data[] = { 0x63, 0xd7, 0x5a, 0x31, 0xa4, 0x46 };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));

    UInt24Parser p1(p->data().begin(),&p->data());
    UInt16Parser p2(p->data().begin()+3,&p->data());

    BOOST_CHECK_EQUAL( ~p1, 4288424101u );
    BOOST_CHECK ( !!p1 );
    BOOST_CHECK_EQUAL( -p1, -6543194u );

    p1 += 0x10;
    BOOST_CHECK_EQUAL( p1, 6543210u );
    p1 -= 0x10;
    BOOST_CHECK_EQUAL( p1, 6543194u );

    p1 += p2;
    BOOST_CHECK_EQUAL( p1, 6555902u );
    p2 += p1;
    // Here some idiotic automatic promotion from unsigned short ->
    // int happens in the first macro parameter ... hrmpf ...
    BOOST_CHECK_EQUAL( p2, 15010 );

    BOOST_CHECK_EQUAL( ++p1, 6555903u );
    BOOST_CHECK_EQUAL( p1++, 6555903u );
    BOOST_CHECK_EQUAL( p1, 6555904u );
    BOOST_CHECK_EQUAL( --p1, 6555903u );
    BOOST_CHECK_EQUAL( p1--, 6555903u );
    BOOST_CHECK_EQUAL( p1, 6555902u );

    p1 = 0x123456u;
    BOOST_CHECK_EQUAL( p->data()[0], 0x12 );
    BOOST_CHECK_EQUAL( p->data()[1], 0x34 );
    BOOST_CHECK_EQUAL( p->data()[2], 0x56 );
    BOOST_CHECK_EQUAL( p->data()[3], 0x3a );

    // I stop here ... this is absolutely identical for all other
    // operators and all other integer types. If really some error pops
    // up, I'll add a check here ...
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
