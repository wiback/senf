// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief RestrictedInt.test unit tests */

//#include "RestrictedInt.test.hh"
//#include "RestrictedInt.test.ih"

// Custom includes
#include "RestrictedInt.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct TestIntTag {};
    typedef senf::RestrictedInt<int,TestIntTag> TestInt;
}

SENF_AUTO_UNIT_TEST(restrictedInt)
{
    TestInt i1;
    TestInt i2(10);

    BOOST_CHECK_EQUAL( i1, TestInt(0) );
    BOOST_CHECK_EQUAL( i2, TestInt(10) );

    BOOST_CHECK_EQUAL( i1+i2, TestInt(10) );

    i1 = TestInt(6);

    BOOST_CHECK_EQUAL( i2-i1, TestInt(4) );

    i2 = TestInt(0);

    BOOST_CHECK_EQUAL( -i1, TestInt(-6) );
    BOOST_CHECK( static_cast<bool>(i1) );
    BOOST_CHECK( static_cast<bool>(!i2) );

    BOOST_CHECK_EQUAL( i1 ++, TestInt(6) );
    BOOST_CHECK_EQUAL( ++ i1, TestInt(8) );

    BOOST_CHECK_EQUAL( i1 * TestInt(2), TestInt(16) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 / TestInt(2), TestInt(4) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 >> TestInt(1), TestInt(4) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 << TestInt(1), TestInt(16) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 & TestInt(2), TestInt(0) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 | TestInt(2), TestInt(10) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );
    BOOST_CHECK_EQUAL( i1 ^ TestInt(2), TestInt(10) );
    BOOST_CHECK_EQUAL( i1, TestInt(8) );

    BOOST_CHECK_EQUAL( ~~i1, i1 );
    SENF_CHECK_NOT_EQUAL( ~i1, i1 );

    BOOST_CHECK( i1 > i2 );
    BOOST_CHECK( i2 < i1 );
    BOOST_CHECK( i1 >= i2 );
    BOOST_CHECK( i2 <= i1 );
    BOOST_CHECK( i1 == TestInt(8) );
    BOOST_CHECK( i1 != i2 );

    BOOST_CHECK_EQUAL( i1.value(), 8 );

    std::stringstream ss;
    ss << i1;
    BOOST_CHECK_EQUAL( ss.str(), "8" );
    ss >> i2;
    BOOST_CHECK_EQUAL( i2, TestInt(8) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
