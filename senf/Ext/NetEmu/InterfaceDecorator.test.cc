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
    \brief InterfaceDecorator unit tests */

#include "InterfaceDecorator.hh"

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/Scheduler/Scheduler.hh>
#include "WLAN/EmulatedWLANInterface.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void runPPI(senf::ClockService::clock_type t) {
        senf::scheduler::TimerEvent timeout (
                "test-timeout", &senf::scheduler::terminate, senf::scheduler::now() + t);
        senf::ppi::run();
    }

    class Counter
        : public senf::ppi::module::MonitorModule<>
    {
        SENF_PPI_MODULE(Counter);
    public:
        Counter(senf::emu::EmulatedWLANInterface &) : n (0u) {}

        unsigned n;

    private:
        virtual void v_handlePacket(senf::Packet const & p)
            { ++ n; }
    };

    typedef senf::emu::ApplyInterfaceDecorator<Counter, Counter> Decoration;
    typedef Decoration::apply<senf::emu::EmulatedWLANInterface>::type IFace;

}

SENF_AUTO_TEST_CASE(interfaceDecorator)
{
    senf::emu::emulatedFrequencyRegistry().start();

    IFace iface1;
    iface1.interface().registerFrequency(2412000, 20000);
    iface1.interface().frequency(2412000, 20000);
    iface1.interface().registerLegacyModulation(54000);
    senf::ppi::module::debug::ActiveFeederSource source1;
    senf::ppi::module::debug::PassiveSink sink1;
    senf::ppi::connect(source1, iface1);
    senf::ppi::connect(iface1, sink1);
    iface1.interface().enable();

    IFace iface2;
    iface2.interface().registerFrequency(2412000, 20000);
    iface2.interface().frequency(2412000, 20000);
    iface2.interface().registerLegacyModulation(54000);
    senf::ppi::module::debug::ActiveFeederSource source2;
    senf::ppi::module::debug::PassiveSink sink2;
    senf::ppi::connect(source2, iface2);
    senf::ppi::connect(iface2, sink2);
    iface2.interface().enable();

    senf::EthernetPacket ep1 (senf::EthernetPacket::create());
    ep1->source() = iface1.interface().id();
    ep1->destination() = iface2.interface().id();
    ep1->type_length() = 0xffff;
    senf::DataPacket::createAfter(ep1, "Packet 1");
    source1.submit(ep1);
    ep1.finalizeAll();

    senf::EthernetPacket ep2 (senf::EthernetPacket::create());
    ep2->source() = iface2.interface().id();
    ep2->destination() = iface1.interface().id();
    ep2->type_length() = 0xffff;
    senf::DataPacket::createAfter(ep2, "Packet 2");
    source2.submit(ep2);
    ep2.finalizeAll();

    runPPI( senf::ClockService::milliseconds(1000));

    BOOST_REQUIRE_EQUAL( sink1.size(), 1u );
    BOOST_CHECK_EQUAL_COLLECTIONS( sink1.front().data().begin(), sink1.front().data().end(),
                                   ep2.data().begin(), ep2.data().end() );
    BOOST_REQUIRE_EQUAL( sink2.size(), 1u );
    BOOST_CHECK_EQUAL_COLLECTIONS( sink2.front().data().begin(), sink2.front().data().end(),
                                   ep1.data().begin(), ep1.data().end() );

    BOOST_CHECK_EQUAL( iface1.receiveModule().n, 1u );
    BOOST_CHECK_EQUAL( iface1.transmitModule().n, 1u );
    BOOST_CHECK_EQUAL( iface2.receiveModule().n, 1u );
    BOOST_CHECK_EQUAL( iface2.transmitModule().n, 1u );

    BOOST_CHECK_EQUAL( Decoration::receiveModule(iface1).n, 1u );
    BOOST_CHECK_EQUAL( Decoration::transmitModule(iface1).n, 1u );
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
