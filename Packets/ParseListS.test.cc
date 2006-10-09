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

//#include "ParseListS.test.hh"
//#include "ParseListS.test.ih"

// Custom includes
#include "ParseListS.hh"
#include "ParseInt.hh"
#include "ParseVec.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/assign.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::pkf;

namespace {
    template <class Value>
    struct Sentinel_IsZero {
        static bool check(Value v) { return v==0; }
    };
}

BOOST_AUTO_UNIT_TEST(parse_ListS_simple)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x00 };
    typedef unsigned char * iterator;
    typedef Parse_ListS<Parse_UInt8<>,Sentinel_IsZero<unsigned char>,iterator> Parse_UInt8ListS;
    
    Parse_UInt8ListS l (data);
    Parse_UInt8ListS::iterator i (l.begin());
    Parse_UInt8ListS::iterator e (l.end());
    for (iterator c (data); *c; ++c) {
        BOOST_REQUIRE( i!=e );
        BOOST_CHECK_EQUAL( *c, *i );
        ++i;
    }
    BOOST_CHECK( i==e );

    BOOST_CHECK_EQUAL( l.bytes(), 5u );
    BOOST_CHECK_EQUAL( l.size(), 4u );
    BOOST_CHECK( !l.empty() );
}

namespace {
    // LVec is a vector with the length living directly before the vector
    template <class Parser, class SizeParser, class Iterator=nil, class IPacket=nil>
    struct Parse_LVec
        : public Parse_Vector<Parser, SizeParser,Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_LVec<Parser,SizeParser,I,P> parser; };
        typedef typename SizeParser::template rebind<Iterator>::parser sizeParser;

        Parse_LVec(Iterator const & i) 
            : Parse_Vector<Parser,SizeParser,Iterator,IPacket>(sizeParser(i),i+sizeParser::bytes())
        {}
        
        unsigned bytes() const
        { return this->Parse_Vector<Parser,SizeParser,Iterator,IPacket>::bytes() + sizeParser::bytes(); }
        bool check(Iterator const & e) const
        // BEWARE .. this->i() points to the Vector not the SizeParser ... hrmpf ...
        { return e>=this->i() && static_cast<unsigned>(e-this->i())+sizeParser::bytes() >= bytes(); }
    };

    template <class Array>
    struct Sentinel_EmptyArray {
        static bool check(Array a) { return a.empty(); }
    };
}

BOOST_AUTO_UNIT_TEST(parse_ListS_complex)
{
    unsigned char data[] = { 0x02, 0x01, 0x02,
                             0x03, 0x11, 0x12, 0x13,
                             0x04, 0x21, 0x22, 0x23, 0x24,
                             0x00 };

    typedef unsigned char * iterator;
    typedef Parse_LVec<Parse_UInt8<>,Parse_UInt8<>,iterator> Parse_UInt8LVec;
    typedef Parse_ListS<Parse_UInt8LVec,Sentinel_EmptyArray<Parse_UInt8LVec>,iterator> Parse_UInt8LVecListS;

    Parse_UInt8LVecListS l (data);
    BOOST_CHECK( l.check(data+13) );

    Parse_UInt8LVecListS::iterator i (l.begin());
    Parse_UInt8LVecListS::iterator e (l.end());
    for (unsigned n (0); n<3; ++n) {
        BOOST_REQUIRE( i!=e );
        BOOST_CHECK_EQUAL( i->size(), n+2 );
        Parse_UInt8LVec::iterator j (i->begin());
        Parse_UInt8LVec::iterator je (i->end());
        for (unsigned m (0); m<n+2; ++m, ++j) {
            BOOST_CHECK( j!=je );
            BOOST_CHECK_EQUAL( static_cast<unsigned>(*j), 16*n+m+1 );
        }
        BOOST_CHECK( j==je );
        ++i;
    }
    BOOST_CHECK( i==e );
    
    BOOST_CHECK_EQUAL( l.size(), 3u );
    BOOST_CHECK_EQUAL( l.bytes(), 13u );
    BOOST_CHECK( !l.empty() );
}

BOOST_AUTO_UNIT_TEST(parse_ListS_wrapper)
{
    typedef std::vector<unsigned char> Container;
    typedef Container::iterator iterator;
    typedef Parse_LVec<Parse_UInt8<>,Parse_UInt8<>,iterator> Parse_UInt8LVec;
    typedef Parse_ListS<Parse_UInt8LVec, Sentinel_EmptyArray<Parse_UInt8LVec>,iterator> Parse_UInt8LVecListS;
    typedef Parse_UInt8LVecListS::wrapper<Container>::t Parse_UInt8LVecListSWrap;

    using namespace boost::assign;

    Container data;
    data +=
        0x02, 0x01, 0x02,
        0x03, 0x11, 0x12, 0x13,
        0x04, 0x21, 0x22, 0x23, 0x24,
        0x00;

    Parse_UInt8LVecListS l (data.begin());
    Parse_UInt8LVecListSWrap w (l,data);

    BOOST_CHECK_EQUAL( w.size(), 3u );
    BOOST_CHECK ( !w.empty() );
    BOOST_CHECK ( w.begin() != w.end() );
    BOOST_CHECK ( w.range() == std::make_pair(w.begin(), w.end()) );

#if 0
    unsigned char newdata[] = { 0x01, 0x00 };

    w.insert(w.begin(),Parse_UInt8LVec::rebind<unsigned char*>::parser(newdata));
    BOOST_CHECK_EQUAL( w.size(), 4u );
    BOOST_CHECK_EQUAL( w.begin()->size(), 1u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>((*w.begin())[0]), 0x00u );
#endif
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
