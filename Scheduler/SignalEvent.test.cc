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
    \brief SignalEvent.test unit tests */

//#include "SignalEvent.test.hh"
//#include "SignalEvent.test.ih"

// Custom includes
#include "SignalEvent.hh"

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

BOOST_AUTO_UNIT_TEST(signalDispatcher)
{
    senf::scheduler::detail::FdManager::instance().timeout(1000);
    senf::scheduler::SignalEvent sig (SIGUSR1, &handler);

    SENF_CHECK_NO_THROW( sig.disable() );
    SENF_CHECK_NO_THROW( sig.enable() );
    SENF_CHECK_NO_THROW( sig.action(&handler) );
    BOOST_CHECK( sig.enabled() );
    BOOST_CHECK( ! called );
    ::kill(::getpid(), SIGUSR1);
    sleep(1);
    SENF_CHECK_NO_THROW( senf::scheduler::detail::SignalDispatcher::instance().unblockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
    SENF_CHECK_NO_THROW( senf::scheduler::detail::SignalDispatcher::instance().blockSignals() );
    SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
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
