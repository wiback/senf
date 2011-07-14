// $Id$
//
// Copyright (C) 2008
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief EventManager unit tests */

//#include "EventManager.test.hh"
//#include "EventManager.test.ih"

// Custom includes
#include "EventManager.hh"
#include "FdEvent.hh"
#include "TimerEvent.hh"
#include "SignalEvent.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(eventManager)
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
