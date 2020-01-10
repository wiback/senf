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
    \brief ActiveFeeder unit tests */

#include "ActiveFeeder.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include "DebugModules.hh"
#include "Setup.hh"
#include "CloneSource.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(activeFeeder)
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
              << (sink.size()*1e9)/ (senf::ClockService::now() - start)
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
