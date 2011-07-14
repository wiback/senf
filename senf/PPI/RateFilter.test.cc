// $Id$
//
// Copyright (C) 2009
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief RateFilter unit tests */

// Custom includes
#include "RateFilter.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Utils/membind.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    void timeout() {
        senf::scheduler::terminate();
    }
}

SENF_AUTO_UNIT_TEST(rateFilter)
{
    module::RateFilter rateFilter ( senf::ClockService::milliseconds(100) );
    debug::PassiveSource source;
    debug::PassiveSink sink;

    ppi::connect(source, rateFilter);
    ppi::connect(rateFilter, sink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));
    for (int i=0; i<10; i++)
        source.submit(p);

    senf::scheduler::TimerEvent timer (
        "rateFilter test timer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(250));

    senf::ppi::run();

    BOOST_CHECK_EQUAL( rateFilter.interval(), senf::ClockService::milliseconds(100) );
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

SENF_AUTO_UNIT_TEST(rateFilter_changeInterval)
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

    senf::scheduler::TimerEvent timeoutTimer (
        "rateFilter test timer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(675));

    RateFilter_IntervalChanger intervalChanger (rateFilter);
    senf::scheduler::TimerEvent timer ( "RateFilter_IntervalChanger timer",
        senf::membind(&RateFilter_IntervalChanger::changeInterval, intervalChanger),
        senf::ClockService::now() + senf::ClockService::milliseconds(250));

    senf::ppi::run();

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
