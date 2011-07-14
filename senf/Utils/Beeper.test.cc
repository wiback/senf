// $Id$
//
// Copyright (C) 2010
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
    \brief Beeper unit tests */

// Custom includes
#include "Beeper.hh"
#include <senf/Scheduler/Scheduler.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(beep_beep)
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
