// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief ClockService.test unit tests */

//#include "ClockService.test.hh"
//#include "ClockService.test.ih"

// Custom includes
#include "ClockService.hh"
#include <errno.h>

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    bool is_close_clock(senf::ClockService::clock_type a, senf::ClockService::clock_type b, 
                        unsigned long delta)
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

BOOST_AUTO_UNIT_TEST(clockService)
{
    senf::ClockService::restart(); // So we know, when the signal will be delivered
    
    senf::ClockService::clock_type t1 (senf::ClockService::now());
    delay(200);
    senf::ClockService::clock_type t2 (senf::ClockService::now());
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::milliseconds(200)) 
                           (t2)
                           (senf::ClockService::milliseconds(100)) );

    t1 = t2;

    // Wait for SIGALRM and let the signal handler do the clock-skew detection
    delay(1*1000);

    t2 = senf::ClockService::now();
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::seconds(1))
                           (t2)
                           (senf::ClockService::milliseconds(500)) );

    t1 = t2;

    delay(200);
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::milliseconds(200))
                           (senf::ClockService::now())
                           (senf::ClockService::milliseconds(100)) );

    // The next check validates that the clock service itimer/heartbeat_ is correctly reset after a
    // clock-skew is detected

    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::milliseconds(200))
                           (senf::ClockService::now()) 
                           (senf::ClockService::milliseconds(100)) );
}

///////////////////////////////cc.e////////////////////////////////////////
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
