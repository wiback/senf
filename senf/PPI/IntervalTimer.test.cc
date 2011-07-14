// $Id$
//
// Copyright (C) 2007
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
    \brief IntervalTimer unit tests */

//#include "IntervalTimer.test.hh"
//#include "IntervalTimer.test.ih"

// Custom includes
#include "IntervalTimer.hh"
#include "Module.hh"
#include "Setup.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace module = senf::ppi::module;
namespace ppi = senf::ppi;

namespace {

    class TimerTest : public module::Module
    {
        SENF_PPI_MODULE(TimerTest);

        ppi::IntervalTimer timer;

        void tick(ppi::IntervalTimer::Event const & event) {
            if ( --n == 0 )
                timer.enabled(false);
        }

        unsigned n;

    public:
        TimerTest(senf::ClockService::int64_type d_, unsigned n_)
        :   timer ( senf::ClockService::milliseconds(d_) ),
            n     ( n_ )
        {
            registerEvent( timer, &TimerTest::tick );
        }
    };

    bool is_close_clock(senf::ClockService::clock_type a, senf::ClockService::clock_type b,
                        senf::ClockService::clock_type delta)
    {
        return (a<b ? b-a : a-b ) < delta;
    }
}

SENF_AUTO_UNIT_TEST(intervalTimer)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");

    TimerTest timer (100,3);
    senf::ClockService::clock_type start (senf::ClockService::now());
    senf::ppi::run();
    if (enabled)
        BOOST_CHECK_PREDICATE( is_close_clock,
                               (senf::ClockService::now())
                               (start+senf::ClockService::milliseconds(300))
                               (senf::ClockService::milliseconds(50)) );
    else
        BOOST_CHECK( true );
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
