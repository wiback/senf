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
    \brief SignalEvent unit tests */

#include "SignalEvent.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool called = false;
    void handler(siginfo_t const &)
    {
        called = true;
    }
}

SENF_AUTO_TEST_CASE(signalDispatcher)
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
