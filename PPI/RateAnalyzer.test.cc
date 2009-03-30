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
    \brief RateAnalyzer unit tests */

//#include "RateAnalyzer.test.hh"
//#include "RateAnalyzer.test.ih"

// Custom includes
#include "RateAnalyzer.hh"
#include "PPI.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    unsigned calls (0u);
    float ppss[] = { 13.333333f, 20.f, 13.333333f, 20.f, 13.333333f, 20.f };

    float pps;
    float bps;

    void collectPPS(float packetsPerSecond)
    {
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

BOOST_AUTO_UNIT_TEST(rateAnalyzer)
{
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

    BOOST_CHECK_CLOSE( pps, 16.67f, .1f );
    BOOST_CHECK_CLOSE( bps, 216.67f, .1f );
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
