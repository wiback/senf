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

//#include "Packet.test.hh"
//#include "Packet.test.ih"

// Custom includes
#include "Packet.hh"
#include "DataPacket.hh"
#include "GenericPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

// Since Packet is abstract, we can only test the Packet interface using
// a simple implementation: DataPacket and GenericPacket.

namespace {

    Packet::byte data[] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                            10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    bool compare(Packet::iterator b, Packet::iterator e, unsigned o=0)
    {
        unsigned i (o);
        for (; b!=e; ++b, ++i)
            if (i>=sizeof(data) || *b != data[i])
                return false;
        return true;
    }

}

BOOST_AUTO_UNIT_TEST(Packet_DataPacket)
{
    Packet::ptr p (Packet::create<DataPacket>(data, data+sizeof(data)));

    BOOST_REQUIRE( p );
    BOOST_CHECK_EQUAL( p->size(), sizeof(data) );
    BOOST_CHECK( compare(p->begin(), p->end()) );

    *p->begin() = 20;
    BOOST_CHECK( !compare(p->begin(), p->end()) );
    BOOST_CHECK_EQUAL( *p->begin(), 20 );

    BOOST_CHECK( !p->next() );
    BOOST_CHECK( !p->prev() );
    BOOST_CHECK_EQUAL( p->head(), p );
    BOOST_CHECK_EQUAL( p->last(), p );

    BOOST_CHECK( p->is<DataPacket>() );
    BOOST_CHECK( p->as<DataPacket>() );
}

BOOST_AUTO_UNIT_TEST(Packet_GenericPacket)
{
    GenericPacket<4,6>::ptr p (Packet::create< GenericPacket<4,6> >(data, data+sizeof(data)));

    // check, that the packet was constructed corretly
    BOOST_REQUIRE( p );
    BOOST_CHECK_EQUAL( p->size(), sizeof(data) );
    BOOST_CHECK( compare(p->begin(), p->end()) );
    BOOST_CHECK_EQUAL( p->header_len(), 4u );
    BOOST_CHECK( compare(p->begin_header(), p->end_header()) );
    BOOST_CHECK_EQUAL( p->trailer_len(), 6u );
    BOOST_CHECK( compare(p->begin_trailer(), p->end_trailer(), sizeof(data)-6) );

    // check the first packet in the interpreter chain
    BOOST_CHECK_EQUAL( p->head(), p );
    BOOST_CHECK( !p->prev() );
    BOOST_CHECK(( p->is< GenericPacket<4,6> >() ));
    BOOST_CHECK( !p->is<DataPacket>() );
    BOOST_CHECK(( !p->is< GenericPacket<4,4> >() ));
    BOOST_CHECK(( p->as< GenericPacket<4,6> >() ));
    BOOST_CHECK( !p->as<DataPacket>() );

    // check the next packet in the interpreter chain
    BOOST_REQUIRE( p->next() );
    BOOST_CHECK( p->next()->is<DataPacket>() );
    BOOST_CHECK(( !p->next()->is< GenericPacket<4,6> >() ));

    // check the contents of the second interpreter
    BOOST_CHECK_EQUAL( p->next()->size(), sizeof(data)-10 );
    BOOST_CHECK( compare(p->next()->begin(), p->next()->end(), 4) );

    // validate, that the two interpreters share the same data
    // container
    *p->next()->begin() = 20;
    BOOST_CHECK( !compare(p->next()->begin(), p->next()->end(), 4) );
    BOOST_CHECK( *p->next()->begin() == 20 );
    BOOST_CHECK( !compare(p->begin(), p->end()) );
    BOOST_CHECK( *(p->begin()+4) == 20 );

    // We need require here. If this fails, p->last() will probably
    // run into an endless loop ...
    BOOST_REQUIRE( !p->next()->next() );
    BOOST_CHECK_EQUAL( p->next(), p->last() );
}

BOOST_AUTO_UNIT_TEST(Packet_Reinterpret)
{
    Packet::ptr p (Packet::create< GenericPacket<4,4> >(data, data+sizeof(data)));

    BOOST_CHECK( p->next()->is<DataPacket>() );
    p->next()->reinterpret< GenericPacket<6> >();
    BOOST_CHECK( p->next()->is< GenericPacket<6> >() );
    BOOST_REQUIRE( p->next()->next() );
    BOOST_CHECK( p->next()->next()->is<DataPacket>() );
    BOOST_CHECK( !p->next()->next()->next() );

    BOOST_CHECK_EQUAL( p->next()->next()->size(), sizeof(data)-14 );
    BOOST_CHECK( compare(p->next()->next()->begin(),
                         p->next()->next()->end(), 10) );

    p = p->reinterpret< GenericPacket<8,2> >();
    BOOST_REQUIRE( p->next() );
    BOOST_CHECK( p->next()->is<DataPacket>() );

    BOOST_CHECK_EQUAL( p->next()->size(), sizeof(data)-10 );
    BOOST_CHECK( compare(p->next()->begin(), p->next()->end(), 8) );
}

BOOST_AUTO_UNIT_TEST(Packet_InsertErase)
{
    Packet::ptr p (Packet::create< GenericPacket<7,3> >(data, data+sizeof(data)));
    p->next()->reinterpret< GenericPacket<4> >();

    BOOST_CHECK_EQUAL( p->size(), 20u );
    BOOST_CHECK_EQUAL( p->next()->size(), 10u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 6u );

    BOOST_CHECK_EQUAL( p->next()->next()->begin()[0], 11 );
    BOOST_CHECK_EQUAL( p->end()[-1], 19 );
    BOOST_CHECK_EQUAL( p->next()->end()[-1], 16 );
    BOOST_CHECK_EQUAL( p->next()->next()->end()[-1], 16 );

    p->next()->insert(p->next()->begin()+2, data, data+6);

    BOOST_CHECK_EQUAL( p->size(), 26u );
    BOOST_CHECK_EQUAL( p->next()->size(), 16u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 6u );

    BOOST_CHECK( compare(p->begin(), p->begin()+9) );
    BOOST_CHECK( compare(p->begin()+9, p->begin()+15) );
    BOOST_CHECK( compare(p->begin()+15, p->end(), 9) );
    BOOST_CHECK( compare(p->next()->begin(), p->next()->begin()+2, 7) );
    BOOST_CHECK( compare(p->next()->begin()+2, p->next()->begin()+8) );
    BOOST_CHECK( compare(p->next()->begin()+8, p->next()->end(), 9) );
    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->end(), 11) );

    p->next()->erase( p->next()->begin()+2, p->next()->begin()+8 );

    BOOST_CHECK_EQUAL( p->size(), 20u );
    BOOST_CHECK_EQUAL( p->next()->size(), 10u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 6u );

    BOOST_CHECK( compare(p->begin(), p->end()) );
    BOOST_CHECK( compare(p->next()->begin(), p->next()->end(), 7) );
    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->end(), 11) );

    p->next()->insert(p->next()->begin()+4, data, data+2);

    BOOST_CHECK_EQUAL( p->size(), 22u );
    BOOST_CHECK_EQUAL( p->next()->size(), 12u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 6u );

    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->end(), 11) );

    p->next()->erase(p->next()->begin()+4, p->next()->begin()+6);

    BOOST_CHECK_EQUAL( p->size(), 20u );
    BOOST_CHECK_EQUAL( p->next()->size(), 10u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 6u );

    BOOST_CHECK( compare(p->begin(), p->end()) );
    BOOST_CHECK( compare(p->next()->begin(), p->next()->end(), 7) );
    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->end(), 11) );

    p->next()->next()->insert(p->next()->begin()+5, data, data+4);

    BOOST_CHECK_EQUAL( p->size(), 24u );
    BOOST_CHECK_EQUAL( p->next()->size(), 14u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 10u );

    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->begin()+1, 11) );
    BOOST_CHECK( compare(p->next()->next()->begin()+1, p->next()->next()->begin()+5) );
    BOOST_CHECK( compare(p->next()->next()->begin()+5, p->end(), 12) );

    p->next()->erase(p->next()->begin()+3, p->next()->begin()+9);

    BOOST_CHECK_EQUAL( p->size(), 18u );
    BOOST_CHECK_EQUAL( p->next()->size(), 8u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 5u );

    BOOST_CHECK( compare(p->next()->next()->begin(), p->next()->next()->end(), 12) );
    BOOST_CHECK( compare(p->begin(), p->begin()+10) );
    BOOST_CHECK( compare(p->begin()+10, p->end(), 12) );

    p->erase(p->begin()+5, p->end());

    BOOST_CHECK_EQUAL( p->size(), 5u );
    BOOST_CHECK_EQUAL( p->next()->size(), 0u );
    BOOST_CHECK_EQUAL( p->next()->next()->size(), 0u );
}

BOOST_AUTO_UNIT_TEST(Packet_new)
{
    Packet::ptr p (Packet::create< GenericPacket<10,4> >());
    BOOST_CHECK_EQUAL(p->size(), 14u);
    Packet::ptr p2 (Packet::create< GenericPacket<2,2> >(p));
    BOOST_CHECK_EQUAL(p2->size(),18u);
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
