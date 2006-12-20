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

//#include "ParserBase.test.hh"
//#include "ParserBase.test.ih"

// Custom includes
#include "ParserBase.hh"
#include "Packet.hh"
#include <boost/static_assert.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

// The non-inherited Version is extensively tested in PaseInt.test.hh

BOOST_AUTO_UNIT_TEST(parserBase_inherited)
{
    // TODO: Implement
}

namespace {

    template <class Iterator=senf::nil, class IPacket=senf::nil>
    struct Parse_Test : public senf::ParserBase<Iterator,IPacket>
    {
        template <class I=senf::nil, class P=senf::nil>
        struct rebind { typedef Parse_Test<I,P> parser; };
        typedef Iterator byte_iterator;
        
        Parse_Test() {}
        Parse_Test(Iterator const & i) : senf::ParserBase<Iterator,IPacket>(i) {}
        
        static unsigned bytes() { return 14; }

        ///////////////////////////////////////////////////////////////////////////

    };

    template <class Iterator=senf::nil, class IPacket=senf::nil>
    struct Parse_Test2 : public senf::ParserBase<Iterator,IPacket>
    {
        template <class I=senf::nil, class P=senf::nil>
        struct rebind { typedef Parse_Test<I,P> parser; };
        typedef Iterator byte_iterator;
        
        Parse_Test2() {}
        Parse_Test2(Iterator const & i) : senf::ParserBase<Iterator,IPacket>(i) {}
        
        unsigned bytes() const { return 14; }
        static unsigned check(Iterator a, Iterator b)
            { return true; }
        static unsigned min_bytes() { return 10; }

        ///////////////////////////////////////////////////////////////////////////

    };

}

BOOST_AUTO_UNIT_TEST(parserBase_construction)
{
    BOOST_STATIC_ASSERT( senf::Parser_traits< Parse_Test<> >::fixed_size );
    BOOST_STATIC_ASSERT( ! senf::Parser_traits< Parse_Test2<> >::fixed_size );

    BOOST_CHECK_EQUAL( senf::min_bytes< Parse_Test<int> >(), 14u );
    BOOST_CHECK( senf::check< Parse_Test<int> >(0,14) );
    BOOST_CHECK( ! senf::check< Parse_Test<int> >(2,15) );

    BOOST_CHECK_EQUAL( senf::min_bytes< Parse_Test2<int> >(), 10u );
    BOOST_CHECK( senf::check< Parse_Test2<int> >(2,13) );
    BOOST_CHECK( senf::check< Parse_Test2<int> >(2,12) );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
