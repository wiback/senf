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
    \brief ClockService unit tests */

#include "ClockService.hh"

// Custom includes
#include <errno.h>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool is_close_clock(senf::ClockService::clock_type a, senf::ClockService::clock_type b,
                        senf::ClockService::clock_type delta)
    {
        return (a<b ? b-a : a-b ) < delta;
    }

    void delay(unsigned long milliseconds)
    {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        while (nanosleep(&ts,&ts) < 0 && errno == EINTR) ;
    }

}

SENF_AUTO_TEST_CASE(clockService)
{
    BOOST_CHECK( senf::ClockService::abstime(senf::ClockService::clock_type(0)).is_not_a_date_time());

    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");
    BOOST_CHECK( true );

    senf::ClockService::restart(); // So we know, when the signal will be delivered

    senf::ClockService::clock_type t1 (senf::ClockService::now());
    delay(200);
    senf::ClockService::clock_type t2 (senf::ClockService::now());
    if (enabled)
        BOOST_CHECK_PREDICATE( is_close_clock,
                               (t1 + senf::ClockService::milliseconds(200))
                               (t2)
                               (senf::ClockService::milliseconds(100)) );

    t1 = t2;

    // Wait for SIGALRM and let the signal handler do the clock-skew detection
    delay(1*1000);

    t2 = senf::ClockService::now();
    if (enabled)
        BOOST_CHECK_PREDICATE( is_close_clock,
                               (t1 + senf::ClockService::seconds(1))
                               (t2)
                               (senf::ClockService::milliseconds(500)) );

    t1 = t2;

    delay(200);
    if (enabled)
        BOOST_CHECK_PREDICATE( is_close_clock,
                               (t1 + senf::ClockService::milliseconds(200))
                               (senf::ClockService::now())
                               (senf::ClockService::milliseconds(100)) );

    // The next check validates that the clock service itimer/heartbeat_ is correctly reset after a
    // clock-skew is detected

    if (enabled)
        BOOST_CHECK_PREDICATE( is_close_clock,
                               (t1 + senf::ClockService::milliseconds(200))
                               (senf::ClockService::now())
                               (senf::ClockService::milliseconds(100)) );

    BOOST_CHECK_EQUAL( senf::ClockService::seconds(1),  senf::ClockService::milliseconds(1000) );
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
