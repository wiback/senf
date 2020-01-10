//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief RateFilter unit tests */

#include "RateFilter.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    void runPPI(senf::ClockService::clock_type t) {
        senf::scheduler::TimerEvent timeout(
                "emulatedDvbInterface test timer", &senf::scheduler::terminate, senf::scheduler::now() + t);
        senf::ppi::run();
    }
}

SENF_AUTO_TEST_CASE(rateFilter)
{
    module::RateFilter rateFilter ( senf::ClockService::milliseconds(200) );
    debug::PassiveSource source;
    debug::PassiveSink sink;

    ppi::connect(source, rateFilter);
    ppi::connect(rateFilter, sink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));
    for (int i=0; i<10; i++)
        source.submit(p);

    runPPI( senf::ClockService::milliseconds(500));

    BOOST_CHECK_EQUAL( rateFilter.interval(), senf::ClockService::milliseconds(200) );
    BOOST_CHECK_EQUAL( sink.size(), 2u );
}

namespace {
    // just a helper class for the test
    struct RateFilter_IntervalChanger {
        module::RateFilter & rateFilter_;
        RateFilter_IntervalChanger( module::RateFilter & rateFilter)
            : rateFilter_( rateFilter) {};
        void changeInterval() {
            rateFilter_.interval( senf::ClockService::milliseconds(200));
        }
    };
}

SENF_AUTO_TEST_CASE(rateFilter_changeInterval)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    if (! enabled) {
        BOOST_WARN_MESSAGE(false, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");
    }

    module::RateFilter rateFilter ( senf::ClockService::milliseconds(100) );
    debug::PassiveSource source;
    debug::PassiveSink sink;

    ppi::connect(source, rateFilter);
    ppi::connect(rateFilter, sink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));
    for (int i=0; i<10; i++)
        source.submit(p);

    RateFilter_IntervalChanger intervalChanger (rateFilter);
    senf::scheduler::TimerEvent timer ( "RateFilter_IntervalChanger timer",
        senf::membind(&RateFilter_IntervalChanger::changeInterval, intervalChanger),
        senf::ClockService::now() + senf::ClockService::milliseconds(250));

    runPPI( senf::ClockService::milliseconds(675));

    BOOST_CHECK_EQUAL( rateFilter.interval(), senf::ClockService::milliseconds(200) );
    if (enabled)
        BOOST_CHECK_EQUAL( sink.size(), 4);
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
