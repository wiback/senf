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
    \brief SignalDispatcher.test unit tests */

//#include "SignalDispatcher.test.hh"
//#include "SignalDispatcher.test.ih"

// Custom includes
#include "SignalDispatcher.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    bool called = false;
    void handler(siginfo_t const &)
    {
        called = true;
    }

}

#if 0
// We can't test this when testing the Scheduler since the Scheduler instance
// already uses the only SignalDispatcher instance allowed ...

BOOST_AUTO_UNIT_TEST(signalDispatcher)
{
    senf::scheduler::FdManager manager;
    senf::scheduler::FIFORunner runner;
    senf::scheduler::SignalDispatcher dispatcher (manager, runner);
    manager.timeout(1000);

    SENF_CHECK_NO_THROW( dispatcher.add(SIGUSR1, &handler) );
    BOOST_CHECK( ! called );
    ::kill(::getpid(), SIGUSR1);
    sleep(1);
    SENF_CHECK_NO_THROW( dispatcher.unblockSignals() );
    SENF_CHECK_NO_THROW( manager.processOnce() );
    SENF_CHECK_NO_THROW( dispatcher.blockSignals() );
    SENF_CHECK_NO_THROW( runner.run() );
    BOOST_CHECK( called );

    SENF_CHECK_NO_THROW( dispatcher.remove(SIGUSR1) );
}

#endif

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
