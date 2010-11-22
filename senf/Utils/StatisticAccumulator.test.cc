// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief StatisticAccumulator non-inline template implementation  */

// Custom includes

#include "StatisticAccumulator.hh"
#include "Statistics.hh"
#include "auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#define prefix_
///////////////////////////////ct.p////////////////////////////////////////
SENF_AUTO_UNIT_TEST(StatisticAccumulator)
{
    senf::Statistics s;

    senf::StatisticAccumulatorInt sa;

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

    BOOST_CHECK_EQUAL( sa.max(), 0);
    BOOST_CHECK_EQUAL( sa.min(), 0);
    BOOST_CHECK( (boost::math::isnan)( sa.avg()));
    BOOST_CHECK_EQUAL( sa.last_avg(), 5.0);
    BOOST_CHECK( (boost::math::isnan)( sa.stddev()));
    BOOST_CHECK_EQUAL( sa.count(), 0);




}
///////////////////////////////ct.e////////////////////////////////////////
#undef prefix_
