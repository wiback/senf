// $Id$
//
// Copyright (C) 2010
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
//   Stefan Bund <g0dil@berlios.de>

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
