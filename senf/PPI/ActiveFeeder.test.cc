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
    \brief ActiveFeeder unit tests */

//#include "ActiveFeeder.test.hh"
//#include "ActiveFeeder.test.ih"

// Custom includes
#include "ActiveFeeder.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include "CloneSource.hh"
#include <senf/Scheduler/Scheduler.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;
namespace module = senf::ppi::module;
namespace scheduler = senf::scheduler;

namespace {
    void run(senf::ClockService::clock_type t) {
        scheduler::TimerEvent timeout (
                "test-timeout", &scheduler::terminate, scheduler::now() + t);
        ppi::run();
    }
}

SENF_AUTO_UNIT_TEST(activeFeeder)
{
    senf::PacketData::byte data[] = { 0xab };
    senf::Packet p (senf::DataPacket::create(data));

    module::CloneSource source (p);
    debug::PassiveSink sink;
    module::ActiveFeeder feeder;

    ppi::connect( source, feeder );
    ppi::connect( feeder, sink   );

    senf::ClockService::clock_type start (senf::ClockService::now());
    run( senf::ClockService::seconds(1));
    std::cerr << "\nActiveFeeder: "
              << (sink.size()*1e9)/(senf::ClockService::now()-start)
              << " packets/s" << std::endl;
    BOOST_CHECK( sink.size() > 0);
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
