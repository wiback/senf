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
    \brief Beeper unit tests */

#include "Beeper.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(beep_beep)
{
    char const * enabled (getenv("SENF_BEEPER_TEST"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_BEEPER_TEST to run the Beeper unit test");
    if (! enabled)
        return;

    senf::Beeper myBeeper;
    myBeeper.beep_sync(440.0, 200);
    myBeeper.beep_sync(300.7, 400, 2, 100);

    sleep( 1);

    senf::scheduler::TimerEvent timer (
            "Beeper unit test timer", &senf::scheduler::terminate,
            senf::ClockService::now() + senf::ClockService::seconds(2));

    myBeeper.beep_async(300.7, 400, 2, 100);
    senf::scheduler::process();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
