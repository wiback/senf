// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief Beeper unit tests */

// Custom includes
#include "Beeper.hh"
#include <senf/Scheduler/Scheduler.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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

///////////////////////////////cc.e////////////////////////////////////////
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
