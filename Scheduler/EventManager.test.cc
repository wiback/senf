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
    \brief EventManager.test unit tests */

//#include "EventManager.test.hh"
//#include "EventManager.test.ih"

// Custom includes
#include "EventManager.hh"
#include "FdEvent.hh"
#include "TimerEvent.hh"
#include "SignalEvent.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(eventManager)
{
    senf::scheduler::FdEvent fdEvent ("fdEvent", 0);
    senf::scheduler::TimerEvent timer ("timer", 0);
    senf::scheduler::SignalEvent signal (SIGUSR2, 0, false);

    senf::scheduler::detail::EventManager & em (senf::scheduler::detail::EventManager::instance());

    senf::scheduler::detail::EventManager::iterator i (em.begin());
    BOOST_REQUIRE( i != em.end() );
    BOOST_CHECK( &(*i) == static_cast<senf::scheduler::detail::Event*>(&fdEvent) );
    ++i;
    BOOST_REQUIRE( i != em.end() );
    BOOST_CHECK( &(*i) == static_cast<senf::scheduler::detail::Event*>(&timer) );
    ++i;
    BOOST_REQUIRE( i != em.end() );
    BOOST_CHECK( &(*i) == static_cast<senf::scheduler::detail::Event*>(&signal) );
    ++i;
    BOOST_CHECK( i == em.end() );
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
