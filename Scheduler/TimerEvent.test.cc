// $Id$
//
// Copyright (C) 2008 
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
    \brief TimerEvent.test unit tests */

//#include "TimerEvent.test.hh"
//#include "TimerEvent.test.ih"

// Custom includes
#include "TimerEvent.hh"

#include "../Utils//auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    bool is_close(senf::ClockService::clock_type a, senf::ClockService::clock_type b)
    {
        return (a<b ? b-a : a-b) < senf::ClockService::milliseconds(100);
    }
    
    bool called = false;
    void handler()
    {
        called = true;
    }

}

BOOST_AUTO_UNIT_TEST(timerDispatcher)
{
    senf::scheduler::detail::FdManager::instance().timeout(1000);

    senf::ClockService::clock_type t (senf::ClockService::now());
    {
        senf::scheduler::TimerEvent timer ("testTimer", &handler, 
                                           t + senf::ClockService::milliseconds(500));
        SENF_CHECK_NO_THROW( timer.disable() );
        SENF_CHECK_NO_THROW( timer.enable() );
        BOOST_CHECK( timer.enabled() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().enable() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().reschedule() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().prepareRun() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().disable() );
        senf::ClockService::clock_type t2 (senf::ClockService::now());
        BOOST_CHECK( called );
        BOOST_CHECK( ! timer.enabled() );
        BOOST_CHECK_PREDICATE( is_close, (t2-t)(senf::ClockService::milliseconds(500)) );

        called=false;
        t = senf::ClockService::now();
        SENF_CHECK_NO_THROW( timer.timeout(t) );
        BOOST_CHECK( timer.enabled() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().enable() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().reschedule() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().prepareRun() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().disable() );
        BOOST_CHECK_PREDICATE( is_close, (t) (senf::ClockService::now()) );
        BOOST_CHECK( called );
    }
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
