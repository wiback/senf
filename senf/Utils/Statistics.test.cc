// $Id$
//
// Copyright (C) 2008 
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
    \brief Statistics unit tests */

//#include "Statistics.test.hh"
//#include "Statistics.test.ih"

// Custom includes
#include "Statistics.hh"
#include "StatisticsTargets.hh"

#include "auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    struct GetRange
    {
        typedef senf::Collector const & first_argument_type;
        typedef unsigned result_type;
        result_type operator()(first_argument_type arg) const
            { return arg.rank(); }
    };

}

BOOST_AUTO_UNIT_TEST(statistics)
{
    senf::Statistics stats;
    senf::log::StringTarget statslog;

    statslog.tag("");
    statslog.showTime(false);
    statslog.showArea(false);
    statslog.showLevel(false);
    statslog.route<senf::StatisticsStream>();

    stats
            .output(2u).connect(
                senf::StatisticsLogger("level0"))
        .collect(4u)
            .output().connect(
                senf::StatisticsLogger("level1"))
            .output(3u).connect(
                senf::StatisticsLogger("averaged1"))
        .collect(3u)
        .collect(2u)
            .output().connect(
                senf::StatisticsLogger("level3"));

    unsigned children1[] = { 4u };
    BOOST_CHECK_EQUAL_COLLECTIONS( 
        boost::make_transform_iterator(stats.collectors().begin(), GetRange()),
        boost::make_transform_iterator(stats.collectors().end(), GetRange()),
        children1, children1 + sizeof(children1)/sizeof(children1[0]) );

    unsigned children2[] = { 3u };
    BOOST_CHECK_EQUAL_COLLECTIONS( 
        boost::make_transform_iterator(stats[4].collectors().begin(), GetRange()),
        boost::make_transform_iterator(stats[4].collectors().end(), GetRange()),
        children2, children2 + sizeof(children2)/sizeof(children2[0]) );

    unsigned children3[] = { 2u };
    BOOST_CHECK_EQUAL_COLLECTIONS( 
        boost::make_transform_iterator(stats[4][3].collectors().begin(), GetRange()),
        boost::make_transform_iterator(stats[4][3].collectors().end(), GetRange()),
        children3, children3 + sizeof(children3)/sizeof(children3[0]) );

    float values[][3] = { 
        { -1.0f,  2.3f,  2.5f }, {  0.3f,  2.4f,  3.8f }, { -1.1f, -0.3f,  0.0f },
        { -0.3f,  3.2f,  3.3f }, {  1.0f,  1.1f,  1.1f }, {  0.5f,  0.5f,  0.5f },
        {  0.0f,  0.0f,  0.0f }, { -2.0f, -1.8f, -1.0f }, {  0.0f,  2.3f,  2.4f },
        {  0.4f,  1.2f,  2.0f }, { -1.0f,  2.3f,  2.5f }, {  0.3f,  2.4f,  3.8f },
        { -1.1f, -0.3f,  0.0f }, { -0.3f,  3.2f,  3.3f }, {  1.0f,  1.1f,  1.1f },
        {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  0.0f }, { -2.0f, -1.8f, -1.0f },
        {  0.0f,  2.3f,  2.4f }, {  0.4f,  1.2f,  2.0f }, { -1.0f,  2.3f,  2.5f },
        {  0.3f,  2.4f,  3.8f }, { -1.1f, -0.3f,  0.0f }, { -0.3f,  3.2f,  3.3f },
        {  1.0f,  1.1f,  1.1f }, {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  0.0f },
        { -2.0f, -1.8f, -1.0f }, {  0.0f,  2.3f,  2.4f }, {  0.4f,  1.2f,  4.0f } };

    for (unsigned i (0); i < sizeof(values)/sizeof(values[0]); ++i)
        stats(values[i][0], values[i][1], values[i][2]);

    BOOST_CHECK_CLOSE( stats.min(), 0.4f, .1f );
    BOOST_CHECK_CLOSE( stats.avg(), 1.2f, .1f );
    BOOST_CHECK_CLOSE( stats.max(), 4.0f, .1f );
    BOOST_CHECK_CLOSE( stats.dev(), 0.0f, .1f );

    BOOST_CHECK_CLOSE( stats[4].min(), -2.0f, .1f );
    BOOST_CHECK_CLOSE( stats[4].avg(), -0.05f, .1f );
    BOOST_CHECK_CLOSE( stats[4].max(), 1.1f, .1f );
    BOOST_CHECK_CLOSE( stats[4].dev(), 1.08282f, .1f );

    BOOST_CHECK_CLOSE( stats[4][3].min(), -2.0f, .1f );
    BOOST_CHECK_CLOSE( stats[4][3].avg(), 1.15f, .1f );
    BOOST_CHECK_CLOSE( stats[4][3].max(),  3.8f, .1f );

    BOOST_CHECK_EQUAL( statslog.str(), 
                       "level0 -1 2.3 2.5 0\n"
                       "level0 -0.35 2.35 3.15 0\n"
                       "level0 -0.4 1.05 1.9 0\n"
                       "level0 -0.7 1.45 1.65 0\n"
                       "level1 -1.1 1.9 3.8 1.31719\n"
                       "averaged1 -1.1 1.9 3.8 1.31719\n"
                       "level0 0.35 2.15 2.2 0\n"
                       "level0 0.75 0.8 0.8 0\n"
                       "level0 0.25 0.25 0.25 0\n"
                       "level0 -1 -0.9 -0.5 0\n"
                       "level1 -2 -0.05 1.1 1.08282\n"
                       "averaged1 -1.55 0.925 2.45 1.20001\n"
                       "level0 -1 0.25 0.7 0\n"
                       "level0 0.2 1.75 2.2 0\n"
                       "level0 -0.3 1.75 2.25 0\n"
                       "level0 -0.35 2.35 3.15 0\n"
                       "level1 -1 2.05 3.8 0.492442\n"
                       "averaged1 -1.36667 1.3 2.9 0.964152\n"
                       "level0 -0.4 1.05 1.9 0\n"
                       "level0 -0.7 1.45 1.65 0\n"
                       "level0 0.35 2.15 2.2 0\n"
                       "level0 0.75 0.8 0.8 0\n"
                       "level1 -1.1 1.125 3.3 1.29687\n"
                       "averaged1 -1.36667 1.04167 2.73333 0.957378\n"
                       "level0 0.25 0.25 0.25 0\n"
                       "level0 -1 -0.9 -0.5 0\n"
                       "level0 -1 0.25 0.7 0\n"
                       "level0 0.2 1.75 2.2 0\n"
                       "level1 -2 0.425 2.4 1.52049\n"
                       "averaged1 -1.36667 1.2 3.16667 1.10327\n"
                       "level0 -0.3 1.75 2.25 0\n"
                       "level0 -0.35 2.35 3.15 0\n"
                       "level0 -0.4 1.05 1.9 0\n"
                       "level0 -0.7 1.45 1.65 0\n"
                       "level1 -1.1 1.9 3.8 1.31719\n"
                       "averaged1 -1.4 1.15 3.16667 1.37818\n"
                       "level3 -2 1.225 3.8 1.45752\n"
                       "level0 0.35 2.15 2.2 0\n"
                       "level0 0.75 0.8 0.8 0\n"
                       "level0 0.25 0.25 0.25 0\n"
                       "level0 -1 -0.9 -0.5 0\n"
                       "level1 -2 -0.05 1.1 1.08282\n"
                       "averaged1 -1.7 0.758333 2.43333 1.30683\n"
                       "level0 -1 0.25 0.7 0\n"
                       "level0 0.2 1.75 3.2 0\n" );

}

///////////////////////////////cc.e////////////////////////////////////////
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
