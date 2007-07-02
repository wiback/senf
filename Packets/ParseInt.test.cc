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

//#include "ParseInt.test.hh"
//#include "ParseInt.test.ih"

// Custom includes
#include "ParseInt.hh"
#include "Packet.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(parseInt_fixedSizes)
{
    unsigned char data[] = { 0x8e, 0x2f, 0x57, 0x12, 0xd1 };
    typedef unsigned char * iterator;

    BOOST_CHECK_EQUAL(Parse_Int8<iterator>(data).value(), -114);
    BOOST_CHECK_EQUAL(Parse_Int8<iterator>(data+1).value(), 47);
    BOOST_CHECK_EQUAL(Parse_UInt8<iterator>(data).value(), 142u);

    BOOST_CHECK_EQUAL(Parse_Int16<iterator>(data).value(), -29137);
    BOOST_CHECK_EQUAL(Parse_Int16<iterator>(data+1).value(), 12119);
    BOOST_CHECK_EQUAL(Parse_UInt16<iterator>(data).value(), 36399u);

    BOOST_CHECK_EQUAL(Parse_Int24<iterator>(data).value(), -7458985);
    BOOST_CHECK_EQUAL(Parse_Int24<iterator>(data+1).value(), 3102482);
    BOOST_CHECK_EQUAL(Parse_UInt24<iterator>(data).value(), 9318231u);

    BOOST_CHECK_EQUAL(Parse_Int32<iterator>(data).value(), -1909500142);
    BOOST_CHECK_EQUAL(Parse_Int32<iterator>(data+1).value(), 794235601);
    BOOST_CHECK_EQUAL(Parse_UInt32<iterator>(data).value(), 2385467154u);
}

BOOST_AUTO_UNIT_TEST(parseInt_bits)
{
    //                       0         1         2         3         4
    //                       012345678901234567890123456789012345678901234567
    //                       --------        --------        --------
    //                       011000111101011101011010001100011010010001000110
    unsigned char data[] = { 0x63,   0xd7,   0x5a,   0x31,   0xa4,   0x46 };
    typedef unsigned char * iterator;

    // 1 byte
    BOOST_CHECK_EQUAL((Parse_UIntField<2,7,iterator>(data).value()), 17u);
    BOOST_CHECK_EQUAL((Parse_IntField<2,7,iterator>(data).value()), -15);
    BOOST_CHECK_EQUAL((Parse_UIntField<3,7,iterator>(data).value()), 1u);
    BOOST_CHECK_EQUAL((Parse_IntField<3,7,iterator>(data).value()), 1);
    BOOST_CHECK_EQUAL((Parse_UIntField<0,8,iterator>(data).value()), 99u);

    // 2 byte
    BOOST_CHECK_EQUAL((Parse_UIntField<5,12,iterator>(data).value()), 61u);
    BOOST_CHECK_EQUAL((Parse_UIntField<0,12,iterator>(data).value()), 1597u);
    BOOST_CHECK_EQUAL((Parse_UIntField<8,13,iterator>(data).value()), 26u);
    BOOST_CHECK_EQUAL((Parse_UIntField<8,16,iterator>(data).value()), 215u);
    BOOST_CHECK_EQUAL((Parse_UIntField<0,16,iterator>(data).value()), 25559u);

    // 3 byte
    BOOST_CHECK_EQUAL((Parse_UIntField<6,20,iterator>(data).value()), 15733u);
    BOOST_CHECK_EQUAL((Parse_IntField<6,20,iterator>(data).value()), -651);
    BOOST_CHECK_EQUAL((Parse_UIntField<13,22,iterator>(data).value()), 470u);

    // 4 byte
    BOOST_CHECK_EQUAL((Parse_UIntField<3,28,iterator>(data).value()), 4027811u);
    BOOST_CHECK_EQUAL((Parse_UIntField<13,38,iterator>(data).value()), 30837865u);
    BOOST_CHECK_EQUAL((Parse_UIntField<8,40,iterator>(data).value()), 3613012388u);
    BOOST_CHECK_EQUAL((Parse_IntField<8,40,iterator>(data).value()), -681954908);

    // 5 byte
    BOOST_CHECK_EQUAL((Parse_UIntField<3,34,iterator>(data).value()), 257779910u);
    BOOST_CHECK_EQUAL((Parse_IntField<13,41,iterator>(data).value()), -21732536);

    // single bit
    BOOST_CHECK_EQUAL((Parse_Flag<32,iterator>(data).value()), true);
    BOOST_CHECK_EQUAL((Parse_Flag<12,iterator>(data).value()), false);
}

BOOST_AUTO_UNIT_TEST(parseInt_assign)
{
    unsigned char data[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    typedef unsigned char * iterator;

    Parse_Int8<iterator>(data).value(0x2f);
    BOOST_CHECK_EQUAL( data[0], 0x2f );

    Parse_Int16<iterator>(data).value(0xa341);
    BOOST_CHECK_EQUAL( data[0], 0xa3 );
    BOOST_CHECK_EQUAL( data[1], 0x41 );

    Parse_Int24<iterator>(data).value(0x234567);
    BOOST_CHECK_EQUAL( data[0], 0x23 );
    BOOST_CHECK_EQUAL( data[1], 0x45 );
    BOOST_CHECK_EQUAL( data[2], 0x67 );

    Parse_Int32<iterator>(data).value(0xfedcba98);
    BOOST_CHECK_EQUAL( data[0], 0xfe );
    BOOST_CHECK_EQUAL( data[1], 0xdc );
    BOOST_CHECK_EQUAL( data[2], 0xba );
    BOOST_CHECK_EQUAL( data[3], 0x98 );

    Parse_IntField<2,6,iterator>(data).value(0x3);
    BOOST_CHECK_EQUAL( data[0], 0xce );
    BOOST_CHECK_EQUAL( data[1], 0xdc );

    Parse_IntField<6,9,iterator>(data).value(0x2);
    BOOST_CHECK_EQUAL( data[0], 0xcd );
    BOOST_CHECK_EQUAL( data[1], 0x5c );
    BOOST_CHECK_EQUAL( data[2], 0xba );

    Parse_IntField<2,21,iterator>(data).value(0x13d75);
    BOOST_CHECK_EQUAL( data[0], 0xc9 );
    BOOST_CHECK_EQUAL( data[1], 0xeb );
    BOOST_CHECK_EQUAL( data[2], 0xaa );
    BOOST_CHECK_EQUAL( data[3], 0x98 );

    Parse_UIntField<4,34,iterator>(data).value(0x268ad497u);
    BOOST_CHECK_EQUAL( (Parse_UIntField<4,34,iterator>(data).value()), 0x268ad497u );
}

BOOST_AUTO_UNIT_TEST(parseInt_operators)
{
    unsigned char data[] = { 0x63, 0xd7, 0x5a, 0x31, 0xa4, 0x46 };

    Parse_UInt24<unsigned char *> p1(data);
    Parse_UInt16<unsigned char *> p2(data+3);

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
    // int happens in the second macro parameter ... hrmpf ...
    BOOST_CHECK_EQUAL( p2, 15010 );

    p1 = 0x123456u;
    BOOST_CHECK_EQUAL( data[0], 0x12 );
    BOOST_CHECK_EQUAL( data[1], 0x34 );
    BOOST_CHECK_EQUAL( data[2], 0x56 );
    BOOST_CHECK_EQUAL( data[3], 0x3a );

    // I stop here ... this is absolutely identical for all other
    // operators and all other integer types. If really some error pops
    // up, I'll add a check here ...
}

namespace {

    template < class P >
    class TestPacket
        : public Packet, public P::template rebind< Packet::iterator,TestPacket<P> >::parser
    {
    public:
        typedef typename P::template rebind<Packet::iterator,TestPacket>::parser parser;
        typedef typename ptr_t<TestPacket>::ptr ptr;

        static bool check(iterator b, iterator e) { return true; }

    private:
        template <class Arg>
        TestPacket(Arg const & arg)
            : Packet(arg) {}

        virtual void v_nextInterpreter() const {}
        virtual void v_finalize() {}
        virtual void v_dump(std::ostream &) const {}

        friend class Packet;
    };

    template < class P >
    typename P::value_type packetCheck()
    {
        unsigned char data[] = { 0x8e, 0x2f, 0x57, 0x12, 0xd1 };
        typename TestPacket<P>::ptr p (Packet::create< TestPacket<P> >(data, data+sizeof(data)));
        return p->value();
    }

}

BOOST_AUTO_UNIT_TEST(parseInt_inherited)
{
    BOOST_CHECK_EQUAL(packetCheck< Parse_Int8<> >(), -114);
    BOOST_CHECK_EQUAL(packetCheck< Parse_UInt8<> >(), 142u);

    BOOST_CHECK_EQUAL(packetCheck< Parse_Int16<> >(), -29137);
    BOOST_CHECK_EQUAL(packetCheck< Parse_UInt16<> >(), 36399u);

    BOOST_CHECK_EQUAL(packetCheck< Parse_Int24<> >(), -7458985);
    BOOST_CHECK_EQUAL(packetCheck< Parse_UInt24<> >(), 9318231u);

    BOOST_CHECK_EQUAL(packetCheck< Parse_Int32<> >(), -1909500142);
    BOOST_CHECK_EQUAL(packetCheck< Parse_UInt32<> >(), 2385467154u);

    BOOST_CHECK_EQUAL((packetCheck< Parse_IntField<3,28> >()), 14873969);
    BOOST_CHECK_EQUAL((packetCheck< Parse_UIntField<3,28> >()), 14873969u);
    BOOST_CHECK_EQUAL((packetCheck< Parse_Flag<11> >()), false);
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
// End:
