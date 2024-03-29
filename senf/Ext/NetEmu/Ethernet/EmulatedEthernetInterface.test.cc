//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EmulatedEthernetInterface unit tests */

#include "EmulatedEthernetInterface.hh"

// Custom includes
#include <unistd.h>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/PPI/DebugModules.hh>
#include <senf/Packets.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    void runPPI(senf::ClockService::clock_type t) {
        senf::scheduler::TimerEvent timeout(
                "test-timeout", &senf::scheduler::terminate, senf::scheduler::now() + t);
        senf::ppi::run();
    }
}

SENF_AUTO_TEST_CASE(emulatedEthernetInterface)
{

    senf::emu::EmulatedEthernetInterface iface1;
    SENF_CHECK_NO_THROW(iface1.vlan(1));
    senf::ppi::module::debug::ActiveFeederSource source1;
    senf::ppi::module::debug::PassiveSink sink1;
    senf::ppi::connect(source1, iface1);
    senf::ppi::connect(iface1, sink1);
    SENF_CHECK_NO_THROW (iface1.enable());

    senf::emu::EmulatedEthernetInterface iface2;
    iface2.vlan(1);
    senf::ppi::module::debug::ActiveFeederSource source2;
    senf::ppi::module::debug::PassiveSink sink2;
    senf::ppi::connect(source2, iface2);
    senf::ppi::connect(iface2, sink2);
    iface2.enable();

    {
        senf::EthernetPacket ep1 (senf::EthernetPacket::create());
        ep1->source() = iface1.id();
        ep1->destination() = iface2.id();
        ep1->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep1, "Packet 1");
        source1.submit(ep1);
        ep1.finalizeAll();

        senf::EthernetPacket ep2 (senf::EthernetPacket::create());
        ep2->source() = iface2.id();
        ep2->destination() = iface1.id();
        ep2->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep2, "Packet 2");
        source2.submit(ep2);
        ep2.finalizeAll();

        runPPI( senf::ClockService::milliseconds(1000));

        BOOST_CHECK_EQUAL( sink1.size(), 1u );
        BOOST_CHECK_EQUAL( sink2.size(), 1u );
        BOOST_CHECK_EQUAL_COLLECTIONS( sink1.front().data().begin(), sink1.front().data().end(),
                                       ep2.data().begin(), ep2.data().end() );
        BOOST_CHECK_EQUAL_COLLECTIONS( sink2.front().data().begin(), sink2.front().data().end(),
                                       ep1.data().begin(), ep1.data().end() );
    }
    {
        iface2.promisc(true);

        senf::EthernetPacket ep1 (senf::EthernetPacket::create());
        ep1->source() = iface1.id();
        ep1->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep1, "Packet 1");
        source1.submit(ep1);
        ep1.finalizeAll();

        senf::EthernetPacket ep2 (senf::EthernetPacket::create());
        ep2->source() = iface2.id();
        ep2->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep2, "Packet 2");
        source2.submit(ep2);
        ep2.finalizeAll();

        runPPI( senf::ClockService::milliseconds(1000));

        BOOST_CHECK_EQUAL( sink1.size(), 1u );
        BOOST_CHECK_EQUAL( sink2.size(), 2u );
    }
    {
        senf::EthernetPacket ep1 (senf::EthernetPacket::create());
        ep1->source() = iface1.id();
        ep1->destination() = iface2.id();
        ep1->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep1, "Packet 1");
        source1.submit(ep1);
        ep1.finalizeAll();

        senf::EthernetPacket ep2 (senf::EthernetPacket::create());
        ep2->source() = iface2.id();
        ep2->destination() = iface1.id();
        ep2->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep2, "Packet 2");
        source2.submit(ep2);
        ep2.finalizeAll();

        iface1.mtu(ep1.next().size());
        iface2.mtu(ep2.next().size()-1);

        runPPI( senf::ClockService::milliseconds(1000));

        BOOST_CHECK_EQUAL( sink1.size(), 1u );
        BOOST_CHECK_EQUAL( sink2.size(), 3u );
    }
    {
         iface2.promisc(false);
         senf::EthernetPacket ep1 (senf::EthernetPacket::create());
         ep1->source() = iface1.id();
         ep1->destination() = senf::MACAddress::from_string("33:33:00:12:34:56");
         ep1->type_length() = 0xffff;
         senf::DataPacket::createAfter(ep1, "Packet 1");
         source1.submit(ep1);
         ep1.finalizeAll();

         senf::EthernetPacket ep2 (senf::EthernetPacket::create());
         ep2->source() = iface2.id();
         ep2->destination() = senf::MACAddress::from_string("33:33:00:12:34:56");
         ep2->type_length() = 0xffff;
         senf::DataPacket::createAfter(ep2, "Packet 2");
         source2.submit(ep2);
         ep2.finalizeAll();

         runPPI( senf::ClockService::milliseconds(1000));

         BOOST_CHECK_EQUAL( sink1.size(), 1u );
         BOOST_CHECK_EQUAL( sink2.size(), 4u );
     }
    {
        //testing interface annotation
        senf::EthernetPacket ep1 (senf::EthernetPacket::create());
        ep1->source() = iface1.id();
        ep1->destination() = iface2.id();
        ep1->type_length() = 0xffff;
        senf::DataPacket::createAfter(ep1, "Packet 1");
        source1.submit(ep1);
        ep1.finalizeAll();

        runPPI( senf::ClockService::milliseconds(1000));
        senf::Packet p (sink2.front());
        BOOST_CHECK_EQUAL(p.annotation<senf::emu::annotations::Interface>().value,iface2.id());


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
