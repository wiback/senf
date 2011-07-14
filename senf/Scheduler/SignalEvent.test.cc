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
    \brief SignalEvent unit tests */

//#include "SignalEvent.test.hh"
//#include "SignalEvent.test.ih"

// Custom includes
#include "SignalEvent.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool called = false;
    void handler(siginfo_t const &)
    {
        called = true;
    }
}

SENF_AUTO_UNIT_TEST(signalDispatcher)
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
