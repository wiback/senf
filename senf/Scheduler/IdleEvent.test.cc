// $Id$
//
// Copyright (C) 2009
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
    \brief IdleEvent.test unit tests */

//#include "IdleEvent.test.hh"
//#include "IdleEvent.test.ih"

// Custom includes
#include "Scheduler.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool timeoutFlag (false);

    void timeout()
    {
        timeoutFlag = true;
        senf::scheduler::terminate();
    }

    unsigned calls (0u);

    void cb()
    {
        if (++calls >= 4)
            senf::scheduler::terminate();
    }

}

SENF_AUTO_UNIT_TEST(idleEvent)
{
    senf::scheduler::IdleEvent idle ("idle event unit test", &cb);

    senf::scheduler::TimerEvent watchdog (
        "IdleEvent unit test watchdog", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(100));

    senf::scheduler::process();

    BOOST_CHECK( ! timeoutFlag );
    BOOST_CHECK_EQUAL( calls, 4u );
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
