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


#include "StatisticAccumulator.hh"

// Custom includes
#include <math.h>
#include "Statistics.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
///////////////////////////////ct.p////////////////////////////////////////

SENF_AUTO_TEST_CASE(StatisticAccumulator)
{
    senf::Statistics s;

    senf::StatisticAccumulator<int> sa;

    sa.accumulate( 5);
    sa.accumulate( 3);
    sa.accumulate( 4);
    sa.accumulate( 6);
    sa.accumulate( 7);

    BOOST_CHECK_EQUAL( sa.max(), 7);
    BOOST_CHECK_EQUAL( sa.min(), 3);
    BOOST_CHECK_EQUAL( sa.avg(), 5.0);
    BOOST_CHECK_EQUAL( sa.last_avg(), 0.0);
//    BOOST_CHECK_EQUAL( sa.stddev(), 1.41421354);
    BOOST_CHECK_EQUAL( sa.count(), 5);

    s(0,sa);

    BOOST_CHECK_EQUAL( sa.max(), std::numeric_limits<int>::lowest());
    BOOST_CHECK_EQUAL( sa.min(), std::numeric_limits<int>::max());
//    BOOST_CHECK( (boost::math::isnan)( sa.avg()));
    BOOST_CHECK( ::isnan( sa.avg()) != 0);
    BOOST_CHECK_EQUAL( sa.last_avg(), 5.0);
    BOOST_CHECK( ::isnan( sa.stddev()));
    BOOST_CHECK_EQUAL( sa.count(), 0);
}

///////////////////////////////ct.e////////////////////////////////////////
#undef prefix_
