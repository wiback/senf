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
    \brief RateAnalyzer unit tests */

//#include "RateAnalyzer.test.hh"
//#include "RateAnalyzer.test.ih"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include "RateAnalyzer.hh"
#include "CloneSource.hh"
#include "RateFilter.hh"
#include "Setup.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    unsigned calls (0u);

    float pps;
    float bps;

    void collectPPS(float packetsPerSecond)
    {
        static float ppss[] = { 13.333333f, 20.f, 13.333333f, 20.f, 13.333333f, 20.f };
        // 2 + 3 + 2 + 3 + 2 + 3 packets / interval
        // -> 13.333333 + 20. + 13.333333 + 20. + 13.333333 + 20. / 6 pps
        // -> 16.666666 pps
        // -> 216.66666 bps
        // This test sometime fails when system load is to high ...
        // BOOST_CHECK_CLOSE( packetsPerSecond, ppss[calls], 0.1f );
        pps += packetsPerSecond;
        ++ calls;
        if (calls >= sizeof(ppss) / sizeof(ppss[0]))
            senf::scheduler::terminate();
    }

    void collectBPS(float bytesPerSecond)
    {
        bps += bytesPerSecond;
    }

    void collectSize(float min, float avg, float max)
    {
        BOOST_CHECK_CLOSE(min, 13.f, 0.01f);
        BOOST_CHECK_CLOSE(avg, 13.f, 0.01f);
        BOOST_CHECK_CLOSE(max, 13.f, 0.01f);
    }
}

SENF_AUTO_UNIT_TEST(rateAnalyzer)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    if (! enabled) {
        BOOST_WARN_MESSAGE(false, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");
    }
    senf::DataPacket p (senf::DataPacket::create(13u));
    senf::ppi::module::CloneSource source (p);
    senf::ppi::module::RateFilter filter (senf::ClockService::milliseconds(58u));
    senf::ppi::module::RateAnalyzer analyzer;
    analyzer.startStatistics(senf::ClockService::milliseconds(150u));
    analyzer.signals.packetsPerSecond.connect(&collectPPS);
    analyzer.signals.bytesPerSecond.connect(&collectBPS);
    analyzer.signals.bytesPerPacket.connect(&collectSize);

    senf::ppi::connect(source, filter);
    senf::ppi::connect(filter, analyzer);

    senf::ppi::run();

    BOOST_CHECK_EQUAL( calls, 6u );

    pps /= calls;
    bps /= calls;

    if (enabled) {
        BOOST_CHECK_CLOSE( pps, 16.67f, .1f );
        BOOST_CHECK_CLOSE( bps, 216.67f, .1f );
    }
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
