// $Id$
//
// Copyright (C) 2007
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
///////////////////////////////cc.p////////////////////////////////////////
namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;
namespace module = senf::ppi::module;
namespace scheduler = senf::scheduler;

namespace {
    void run(senf::ClockService::clock_type t) {
        scheduler::TimerEvent timeout(
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
