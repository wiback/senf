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
    \brief IntervalTimer unit tests */

#include "IntervalTimer.hh"

// Custom includes
#include "Module.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(intervalTimer)
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
