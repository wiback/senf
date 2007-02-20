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

//#include "ParseArray.test.hh"
//#include "ParseArray.test.ih"

// Custom includes
#include "ParseArray.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(parseArray_test)
{
    unsigned char data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    typedef unsigned char * iterator;
    typedef Parse_Array<6,Parse_UInt8<>,iterator> Parse_UInt8Array6;
    Parse_UInt8Array6 v (data);
    BOOST_CHECK_EQUAL( v[0], 0x00 );
    BOOST_CHECK_EQUAL( v[5], 0x05 );
    BOOST_CHECK_EQUAL( *v.begin(), 0x00 );
    Parse_UInt8Array6::iterator i1 (v.begin());
    Parse_UInt8Array6::iterator i2 (v.begin());
    ++i1;
    BOOST_CHECK_EQUAL( *i1, 0x01 );
    BOOST_CHECK_EQUAL( i1[-1], 0x00 );
    BOOST_CHECK_EQUAL( i1-i2, 1 );
    BOOST_CHECK_EQUAL( i2-i1, -1 );
    --i1;
    BOOST_CHECK( i1==i2 );
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
