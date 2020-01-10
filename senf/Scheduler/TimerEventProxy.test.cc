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
    \brief TimerEventProxy.test non-inline non-template implementation */

#include "TimerEventProxy.hh"

// Custom includes
#include <boost/random.hpp>
#include "Scheduler.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

namespace {

    int mask = 0;

    void handler(ClockService::clock_type time, int const &id)
    {
        mask = mask + id;
    }

    void run(ClockService::clock_type t) {
        scheduler::TimerEvent timeout(
                "test-timeout", &scheduler::terminate, scheduler::now() + t);
        scheduler::process();
    }

}

SENF_AUTO_TEST_CASE(timerEventProxy)
{
//    // abort on watchdog timeout
//    scheduler::watchdogAbort( true);
//    scheduler::watchdogTimeout(5000);

    ClockService::clock_type now (ClockService::now());
    {
        scheduler::TimerEventProxy<int> timers ("unit-test");

//        timers.add( t + ClockService::milliseconds(10000), 0 , &handler);
        timers.add( now + ClockService::milliseconds(800), 4, &handler);
        timers.add( now + ClockService::milliseconds(200), 1, &handler);
        BOOST_CHECK( timers.remove( 4));
        BOOST_CHECK(! timers.remove( 4));
        BOOST_CHECK_EQUAL( timers.timeout(4), ClockService::clock_type(0));
        timers.add( now + ClockService::milliseconds(700), 2, &handler);

        BOOST_CHECK_EQUAL( timers.timeout(1), now + ClockService::milliseconds(200));
        BOOST_CHECK_EQUAL( timers.timeout(2), now + ClockService::milliseconds(700));

        timers.add( now + ClockService::milliseconds(800), 2, &handler);
        BOOST_CHECK_EQUAL( timers.timeout(2), now + ClockService::milliseconds(800));
        timers.add( now, 4, &handler);
        BOOST_CHECK_EQUAL( timers.numEvents(), 3);

        run( ClockService::milliseconds( 2000));

        BOOST_CHECK_EQUAL( mask, 7);
        BOOST_CHECK_EQUAL( timers.numEvents(), 0);

        timers.add( ClockService::now() + ClockService::milliseconds(800), 1, &handler);
        timers.clear();
        run( ClockService::milliseconds( 2000));
        BOOST_CHECK_EQUAL( mask, 7);
        BOOST_CHECK_EQUAL( timers.numEvents(), 0);
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
