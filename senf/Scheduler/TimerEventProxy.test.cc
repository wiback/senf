// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Jens Moedeker <jens.moedeker@fokus.fraunhofer.de>
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
    \brief TimerEventProxy.test non-inline non-template implementation */

//#include "TimerEventProxy.test.hh"
//#include "TimerEventProxy.test.ih"

// Custom includes
#include "TimerEventProxy.hh"
#include "Scheduler.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include <boost/random.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
namespace {

    int mask = 0;

    void handler( senf::ClockService::clock_type time, int const &id)
    {
        mask = mask + id;
    }

    void run(senf::ClockService::clock_type t) {
        senf::scheduler::TimerEvent timeout(
                "test-timeout", &senf::scheduler::terminate, senf::scheduler::now() + t);
        senf::scheduler::process();
    }

}

SENF_AUTO_UNIT_TEST(timerEventProxy)
{
//    // abort on watchdog timeout
//    senf::scheduler::watchdogAbort( true);
//    senf::scheduler::watchdogTimeout(5000);

    senf::ClockService::clock_type t (senf::ClockService::now());
    {
        senf::scheduler::TimerEventProxy<int> timers;

//        timers.add( t + senf::ClockService::milliseconds(10000), 0 , &handler);
        timers.add( t + senf::ClockService::milliseconds(800), 4, &handler);
        timers.add( t + senf::ClockService::milliseconds(200), 1, &handler);
        BOOST_CHECK( timers.remove( 4));
        BOOST_CHECK(! timers.remove( 4));
        timers.add( t + senf::ClockService::milliseconds(700), 2, &handler);
        
        BOOST_CHECK_EQUAL( timers.timeout(1), t + senf::ClockService::milliseconds(200));
        BOOST_CHECK_EQUAL( timers.timeout(2), t + senf::ClockService::milliseconds(700));
        BOOST_CHECK_EQUAL( timers.timeout(4), 0);

        run( senf::ClockService::milliseconds( 2000));

        BOOST_CHECK( mask == 3);
    }
    
    senf::scheduler::TimerEventProxy<int> timers ("test", senf::console::ScopedDirectory<>());
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
