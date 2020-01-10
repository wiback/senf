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
    \brief PassiveQueue unit tests */

#include "PassiveQueue.hh"

// Custom includes
#include <senf/Packets/Packets.hh>
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

SENF_AUTO_TEST_CASE(passiveQueue)
{
    debug::ActiveSource source;
    module::PassiveQueue queue;
    debug::ActiveSink sink;

    ppi::connect(source,queue);
    ppi::connect(queue,sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());

    BOOST_CHECK( ! sink );
    source.submit(p);
    BOOST_CHECK( sink );
    BOOST_CHECK( ! source );
    BOOST_CHECK_EQUAL( queue.input.queueSize(), 1u );
    BOOST_CHECK( sink.request() == p );
    BOOST_CHECK( source );
    BOOST_CHECK( ! sink );
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
