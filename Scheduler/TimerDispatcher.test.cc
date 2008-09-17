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
    \brief TimerDispatcher.test unit tests */

//#include "TimerDispatcher.test.hh"
//#include "TimerDispatcher.test.ih"

// Custom includes
#include "TimerDispatcher.hh"

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
    senf::scheduler::TimerDispatcher dispatcher (senf::scheduler::FdManager::instance(), senf::scheduler::FIFORunner::instance());
    senf::scheduler::FdManager::instance().timeout(1000);

    senf::ClockService::clock_type t (senf::ClockService::now());
    senf::scheduler::TimerDispatcher::timer_id id;
    SENF_CHECK_NO_THROW(
        id = dispatcher.add( "testTimer", t + senf::ClockService::milliseconds(500), &handler ) );
    SENF_CHECK_NO_THROW( dispatcher.unblockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::FdManager::instance().processOnce() );
    SENF_CHECK_NO_THROW( dispatcher.blockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::FIFORunner::instance().run() );
    senf::ClockService::clock_type t2 (senf::ClockService::now());
    BOOST_CHECK( called );
    BOOST_CHECK_PREDICATE( is_close, (t2-t)(senf::ClockService::milliseconds(500)) );

    SENF_CHECK_NO_THROW( dispatcher.remove(id) );

    called=false;
    t = senf::ClockService::now();
    SENF_CHECK_NO_THROW( dispatcher.add( "testTimer", t, &handler ) );
    SENF_CHECK_NO_THROW( dispatcher.unblockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::FdManager::instance().processOnce() );
    SENF_CHECK_NO_THROW( dispatcher.blockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::FIFORunner::instance().run() );
    BOOST_CHECK_PREDICATE( is_close, (t) (senf::ClockService::now()) );
    BOOST_CHECK( called );
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
