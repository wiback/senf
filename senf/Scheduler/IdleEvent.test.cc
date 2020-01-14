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
    \brief IdleEvent.test unit tests */

#include "Scheduler.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool timeoutFlag (false);

    void timeout()
    {
        timeoutFlag = true;
        senf::scheduler::terminate();
    }

    unsigned calls (0u);

    void cb()
    {
        if (++calls >= 4)
            senf::scheduler::terminate();
    }

}

SENF_AUTO_TEST_CASE(idleEvent)
{
    senf::scheduler::IdleEvent idle ("idle event unit test", &cb);

    senf::scheduler::TimerEvent watchdog (
        "IdleEvent unit test watchdog", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(100));

    senf::scheduler::process();

    BOOST_CHECK( ! timeoutFlag );
    BOOST_CHECK_EQUAL( calls, 4u );
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
