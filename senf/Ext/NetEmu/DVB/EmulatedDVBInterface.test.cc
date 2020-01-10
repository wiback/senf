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
    \brief EmulatedDVBInterface unit tests */

#include "EmulatedDVBInterface.hh"

// Custom includes
#include <unistd.h>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets.hh>
#include <senf/PPI/DebugModules.hh>
#include <senf/Ext/NetEmu/WLAN/EmulatedWLANInterface.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

namespace {
    void runPPI(ClockService::clock_type t) {
        scheduler::TimerEvent timeout(
                "emulatedDvbInterface test timer", &scheduler::terminate, scheduler::now() + t);
        ppi::run();
    }
}

SENF_AUTO_TEST_CASE(emulatedDvbInterface)
{
    emu::emulatedFrequencyRegistry().start();

    emu::EmulatedDVBSenderInterface iface1;
    iface1.registerFrequency(400000, 6000000);
    iface1.tune( emu::DVBModulationParameter::FEC_3_4, emu::DVBModulationParameter::QAM_16, emu::DVBModulationParameter::GUARD_INTERVAL_1_16, 500000);
    ppi::module::debug::ActiveFeederSource source1;
    ppi::connect(source1, iface1);
    iface1.enable();
    BOOST_CHECK(!iface1.canReceive());
    BOOST_CHECK(iface1.canTransmit());

    emu::EmulatedDVBReceiverInterface iface2;
    iface2.registerFrequency(400000, 600000);
    iface2.tune( emu::DVBModulationParameter::FEC_3_4, emu::DVBModulationParameter::QAM_16, emu::DVBModulationParameter::GUARD_INTERVAL_1_16, 500000);
    ppi::module::debug::PassiveSink sink2;
    ppi::connect(iface2, sink2);
    iface2.enable();
    BOOST_CHECK(iface2.canReceive());
    BOOST_CHECK(!iface2.canTransmit());
    EthernetPacket ep1 (EthernetPacket::create());
    ep1->source() = iface1.id();
    ep1->destination() = iface2.id();
    ep1->type_length() = 0xffff;
    DataPacket::createAfter(ep1, "Packet 1");
    ep1.finalizeAll();
    source1.submit(ep1);

    runPPI( ClockService::milliseconds(1000)) ;

     BOOST_CHECK_EQUAL( sink2.size(), 1u );
//    BOOST_CHECK_EQUAL_COLLECTIONS( sink1.front().data().begin(), sink1.front().data().end(),
//                                   ep2.data().begin(), ep2.data().end() );
    BOOST_CHECK_EQUAL_COLLECTIONS( sink2.front().data().begin(), sink2.front().data().end(),
                                   ep1.data().begin(), ep1.data().end() );
}


SENF_AUTO_TEST_CASE(emulatedDvbInterface_WLANconnTest)
{
    emu::emulatedFrequencyRegistry().start();

    emu::EmulatedWLANInterface iface1;
    iface1.registerFrequency(2412000, 20000);
    iface1.frequency(2412000, 20000);
    iface1.registerLegacyModulation(54000);
    ppi::module::debug::ActiveFeederSource source1;
    ppi::module::debug::PassiveSink sink1;
    ppi::module::debug::PassiveSink monitorSink1;
    ppi::connect(source1, iface1);
    ppi::connect(iface1, sink1);
    ppi::connect(iface1.monitor, monitorSink1);
    iface1.enable();

    emu::EmulatedDVBSenderInterface iface2;
    iface2.registerFrequency(2412000);
    iface2.tune( emu::DVBModulationParameter::FEC_3_4, emu::DVBModulationParameter::QAM_16, emu::DVBModulationParameter::GUARD_INTERVAL_1_16, 2412000);
    ppi::module::debug::ActiveFeederSource source2;
    ppi::connect(source2, iface2);
    iface2.enable();

    {
          EthernetPacket ep1 (EthernetPacket::create());
          ep1->source() = iface1.id();
          ep1->destination() = iface2.id();
          ep1->type_length() = 0xffff;
          DataPacket::createAfter(ep1, "Packet 1");
          source1.submit(ep1);
          ep1.finalizeAll();

          EthernetPacket ep2 (EthernetPacket::create());
          ep2->source() = iface2.id();
          ep2->destination() = iface1.id();
          ep2->type_length() = 0xffff;
          DataPacket::createAfter(ep2, "Packet 2");
          ep2.finalizeAll();
          source2.submit(ep2);

          runPPI( ClockService::milliseconds(1000)) ;

          BOOST_CHECK_EQUAL( sink1.size(), 0u );
    }
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
