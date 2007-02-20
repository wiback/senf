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

//#include "ParseVec.test.hh"
//#include "ParseVec.test.ih"

// Custom includes
#include "ParseVec.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/assign.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(parseVec_test)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    typedef unsigned char * iterator;
    typedef Parse_Vector<Parse_UInt16<>,Parse_UInt8<>,iterator> Parse_UInt16Vec;

    Parse_UInt8<iterator> sizeParser (data);
    Parse_UInt16Vec v (sizeParser, data+1);

    BOOST_CHECK_EQUAL( v[0], 0x1011 );
    BOOST_CHECK_EQUAL( v[2], 0x1415 );
    BOOST_CHECK_EQUAL( v.size(), 3u );
    BOOST_CHECK_EQUAL( v.bytes(), 6u );
    data[0] = 0x06;
    BOOST_CHECK_EQUAL( v.size(), 6u );
    BOOST_CHECK_EQUAL( v.bytes(), 12u );

    iterator i (data+1);
    Parse_UInt16Vec::iterator j (v.begin());
    Parse_UInt16Vec::iterator e (v.end());
    for (;j!=e;++j, i+=2)
        BOOST_CHECK_EQUAL( Parse_UInt16<iterator>(i), *j );
    BOOST_CHECK_EQUAL(i, data+13);
}

BOOST_AUTO_UNIT_TEST(parseVec_wrapper)
{
    typedef std::vector<unsigned char> Container;
    typedef Container::iterator iterator;
    typedef Parse_UInt8<iterator> Parse_Size;
    typedef Parse_Vector<Parse_UInt16<>,Parse_Size,iterator> Parse_UInt16Vec;
    typedef Parse_UInt16Vec::wrapper<Container>::t Parse_UInt16VecWrap;

    using namespace boost::assign;

    Container data;
    data +=
        0x03,                                   // size
        0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
        0x20, 0x21,  0x22, 0x23,  0x24, 0x25;

    Parse_Size sizeParser (data.begin());
    Parse_UInt16Vec v (sizeParser, data.begin()+1);
    Parse_UInt16VecWrap w (v,data);

    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[2], 0x1415 );
    BOOST_CHECK_EQUAL( w.size(), 3u );
    data[0] = 0x06;
    BOOST_CHECK_EQUAL( w.size(), 6u );

    {
        iterator i (data.begin()+1);
        Parse_UInt16VecWrap::iterator j (w.begin());
        Parse_UInt16VecWrap::iterator e (w.end());
        for (;j!=e;++j, i+=2)
            BOOST_CHECK_EQUAL( Parse_UInt16<iterator>(i), *j );
        BOOST_CHECK_EQUAL(data.end()-i, 0);
    }

    w.shift(w.begin()+1);
    BOOST_CHECK_EQUAL( w.size(), 7u );
    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[1], 0 );
    BOOST_CHECK_EQUAL( w[2], 0x1213 );

    w.insert(w.begin()+3, 2u, 0xfffe);
    BOOST_CHECK_EQUAL( w.size(), 9u );
    BOOST_CHECK_EQUAL( w[2], 0x1213 );
    BOOST_CHECK_EQUAL( w[3], 0xfffe );
    BOOST_CHECK_EQUAL( w[4], 0xfffe );
    BOOST_CHECK_EQUAL( w[5], 0x1415 );

    w.erase(w.begin()+3, w.begin()+5);
    BOOST_CHECK_EQUAL( w.size(), 7u );

    w.erase(w.begin()+1);
    BOOST_CHECK_EQUAL( w.size(), 6u );

    {
        iterator i (data.begin()+1);
        Parse_UInt16VecWrap::iterator j (w.begin());
        Parse_UInt16VecWrap::iterator e (w.end());
        for (;j!=e;++j, i+=2)
            BOOST_CHECK_EQUAL( Parse_UInt16<iterator>(i), *j );
        BOOST_CHECK_EQUAL(data.end()-i, 0);
    }

    w.clear();
    BOOST_CHECK_EQUAL( w.size(), 0u );
    BOOST_CHECK( w.begin() == w.end() );
    BOOST_CHECK_EQUAL( data.size(), 1u );
}

// This really belongs into ParserBase.test.cc but it's simpler here
BOOST_AUTO_UNIT_TEST(parserTraits_test)
{
    // Really, this could be checked by BOOST_STATIC_ASSERT since
    // it's compile-time ...
    BOOST_CHECK( Parser_traits< Parse_UInt32<> >::fixed_size );
    BOOST_CHECK( (! Parser_traits< Parse_Vector< Parse_UInt16<>,Parse_UInt16<> > >::fixed_size) );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
