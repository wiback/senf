// $Id$
//
// Copyright (C) 2008
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
    \brief Statistics unit tests */

//#include "Statistics.test.hh"
//#include "Statistics.test.ih"

// Custom includes
#include "Statistics.hh"
#include "StatisticsTargets.hh"
#include <boost/tokenizer.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct GetRank
    {
        typedef senf::Collector const & first_argument_type;
        typedef unsigned result_type;
        result_type operator()(first_argument_type arg) const
            { return arg.rank(); }
    };

    struct splitFloats
    {
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        struct Pred {
            bool operator()(std::string const & s) const
                { try { boost::lexical_cast<double>(s); return true; }
                    catch (std::bad_cast &) { return false; } }
        };
        typedef boost::filter_iterator<Pred,Tokenizer::iterator> FilterIterator;
        struct Trafo {
            typedef double result_type;
            result_type operator()(std::string const & s) const
                { return boost::lexical_cast<double>(s); }
        };
        typedef boost::transform_iterator<Trafo,FilterIterator> TransformIterator;

        typedef TransformIterator iterator;
        typedef TransformIterator const_iterator;

        splitFloats(std::string const & s) : s_ (s), sep_ (" \n"), tok_ (s_, sep_) {}
        TransformIterator begin() const
            { return TransformIterator(FilterIterator(tok_.begin(), tok_.end())); }
        TransformIterator end() const
            { return TransformIterator(FilterIterator(tok_.end(), tok_.end())); }

        std::string s_;
        Separator sep_;
        Tokenizer tok_;
    };

    template <class R1, class R2>
    void CHECK_CLOSE_RANGES(R1 const & r1, R2 const & r2, double delta)
    {
        typedef typename boost::range_const_iterator<R1>::type I1;
        typedef typename boost::range_const_iterator<R2>::type I2;

        I1 i1 (boost::begin(r1));
        I1 i1_end (boost::end(r1));
        I2 i2 (boost::begin(r2));
        I2 i2_end (boost::end(r2));

        for (; i1 != i1_end && i2 != i2_end; ++i1, ++i2) {
            BOOST_CHECK_CLOSE(*i1, *i2, delta);
        }

        BOOST_CHECK( i1 == i1_end );
        BOOST_CHECK( i2 == i2_end );
    }
}

#define CHECK_STATS(a, b) CHECK_CLOSE_RANGES(splitFloats(a),splitFloats(b),0.001f)
#define CHECK_STATS_STR(a,b) BOOST_CHECK_EQUAL(a,b)

SENF_AUTO_UNIT_TEST(statistics)
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
        boost::make_transform_iterator(stats.collectors().begin(), GetRank()),
        boost::make_transform_iterator(stats.collectors().end(), GetRank()),
        children1, children1 + sizeof(children1)/sizeof(children1[0]) );

    unsigned children2[] = { 3u };
    BOOST_CHECK_EQUAL_COLLECTIONS(
        boost::make_transform_iterator(stats[4].collectors().begin(), GetRank()),
        boost::make_transform_iterator(stats[4].collectors().end(), GetRank()),
        children2, children2 + sizeof(children2)/sizeof(children2[0]) );

    unsigned children3[] = { 2u };
    BOOST_CHECK_EQUAL_COLLECTIONS(
        boost::make_transform_iterator(stats[4][3].collectors().begin(), GetRank()),
        boost::make_transform_iterator(stats[4][3].collectors().end(), GetRank()),
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
    // 30 entries
    //   -> 30 calls of stats()
    //         (rest 2)
    //         -> 7 calls of stats[4]()
    //              (rest 1)
    //              -> 2 calls of stats[4][3]()
    //                   (rest 0)
    //                   -> 1 call of stats[4][3][2]

    for (unsigned i (0); i < sizeof(values)/sizeof(values[0]); ++i)
        stats(1, values[i][0], values[i][1], values[i][2]);

    BOOST_CHECK_CLOSE( stats.min(), 0.4f, .001f );
    BOOST_CHECK_CLOSE( stats.avg(), 1.2f, .001f );
    BOOST_CHECK_CLOSE( stats.max(), 4.0f, .001f );
    BOOST_CHECK_CLOSE( stats.dev(), 0.0f, .001f );

    BOOST_CHECK_CLOSE( stats[4].min(), -2.0f, .001f );
    BOOST_CHECK_CLOSE( stats[4].avg(), -0.05f, .001f );
    BOOST_CHECK_CLOSE( stats[4].max(), 1.1f, .001f );
    BOOST_CHECK_CLOSE( stats[4].dev(), 1.08282f, .001f );

    BOOST_CHECK_CLOSE( stats[4][3].min(), -2.0f, .001f );
    BOOST_CHECK_CLOSE( stats[4][3].avg(), 1.15f, .001f );
    BOOST_CHECK_CLOSE( stats[4][3].max(),  3.8f, .001f );

    CHECK_STATS( statslog.str(),
            "level0 1 -1 2.3 2.5 0\n"
            "level0 2 -1 2.35 3.8 0\n"
            "level0 2 -1.1 1.05 3.8 0\n"
            "level0 2 -1.1 1.45 3.3 0\n"
            "level1 4 -1.1 1.9 3.8 1.31719\n"
            "averaged1 4 -1.1 1.9 3.8 1.31719\n"
            "level0 2 -0.3 2.15 3.3 0\n"
            "level0 2 0.5 0.8 1.1 0\n"
            "level0 2 0 0.25 0.5 0\n"
            "level0 2 -2 -0.9 0 0\n"
            "level1 4 -2 -0.05 1.1 1.08282\n"
            "averaged1 8 -2 0.925 3.8 1.20001\n"
            "level0 2 -2 0.25 2.4 0\n"
            "level0 2 0 1.75 2.4 0\n"
            "level0 2 -1 1.75 2.5 0\n"
            "level0 2 -1 2.35 3.8 0\n"
            "level1 4 -1 2.05 3.8 0.492442\n"
            "averaged1 12 -2 1.3 3.8 0.964152\n"
            "level0 2 -1.1 1.05 3.8 0\n"
            "level0 2 -1.1 1.45 3.3 0\n"
            "level0 2 -0.3 2.15 3.3 0\n"
            "level0 2 0.5 0.8 1.1 0\n"
            "level1 4 -1.1 1.125 3.3 1.29687\n"
            "averaged1 12 -2 1.04167 3.8 0.957378\n"
            "level0 2 0 0.25 0.5 0\n"
            "level0 2 -2 -0.9 0 0\n"
            "level0 2 -2 0.25 2.4 0\n"
            "level0 2 0 1.75 2.4 0\n"
            "level1 4 -2 0.425 2.4 1.52049\n"
            "averaged1 12 -2 1.2 3.8 1.10327\n"
            "level0 2 -1 1.75 2.5 0\n"
            "level0 2 -1 2.35 3.8 0\n"
            "level0 2 -1.1 1.05 3.8 0\n"
            "level0 2 -1.1 1.45 3.3 0\n"
            "level1 4 -1.1 1.9 3.8 1.31719\n"
            "averaged1 12 -2 1.15 3.8 1.37818\n"
            "level3 24 -2 1.225 3.8 1.45752\n"
            "level0 2 -0.3 2.15 3.3 0\n"
            "level0 2 0.5 0.8 1.1 0\n"
            "level0 2 0 0.25 0.5 0\n"
            "level0 2 -2 -0.9 0 0\n"
            "level1 4 -2 -0.05 1.1 1.08282\n"
            "averaged1 12 -2 0.758333 3.8 1.30683\n"
            "level0 2 -2 0.25 2.4 0\n"
            "level0 2 0 1.75 4 0\n" );

    statslog.clear();
    stats(20, 1, 10.f, 10.f, 10.f, 0.f);
    stats(4, 1, 0.f, 0.f, 0.f, 0.f);

    CHECK_STATS( statslog.str(),
            "level0 2 0.4 5.6 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level0 2 10 10 10 0\n"
            "level1 4 0 5.875 10 8.19554\n"
            "averaged1 12 -2 2.575 10 3.53185\n"
            "level1 1 10 10 10 0\n"
            "averaged1 9 -2 5.275 10 3.09279\n"
            "level1 1 10 10 10 0\n"
            "averaged1 6 0 8.625 10 2.73185\n"
            "level1 1 10 10 10 0\n"
            "averaged1 3 10 10 10 0\n"
            "level1 1 10 10 10 0\n"
            "averaged1 3 10 10 10 0\n"
            "level3 10 -2 7.6375 10 5.04759\n"
            "level0 2 0 5 10 0\n"
            "level0 2 0 0 0 0\n"
            "level0 2 0 0 0 0\n"
            "level0 2 0 0 0 0\n"
            "level1 4 0 5 10 8.66025\n"
            "averaged1 6 0 8.33333 10 2.88675\n" );

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
