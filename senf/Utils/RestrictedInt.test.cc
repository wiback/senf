//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief RestrictedInt.test unit tests */

#include "RestrictedInt.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct TestIntTag {};
    typedef senf::RestrictedInt<int,TestIntTag> TestInt;
}

SENF_AUTO_TEST_CASE(restrictedInt)
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
