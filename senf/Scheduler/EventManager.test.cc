//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EventManager unit tests */

#include "EventManager.hh"

// Custom includes
#include "FdEvent.hh"
#include "TimerEvent.hh"
#include "SignalEvent.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(eventManager)
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
