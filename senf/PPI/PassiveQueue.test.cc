// $Id$
//
// Copyright (C) 2007
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
    \brief PassiveQueue unit tests */

//#include "PassiveQueue.test.hh"
//#include "PassiveQueue.test.ih"

// Custom includes
#include "PassiveQueue.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Packets/Packets.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

SENF_AUTO_UNIT_TEST(passiveQueue)
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
