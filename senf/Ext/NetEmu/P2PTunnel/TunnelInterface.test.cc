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
    \brief TunnelInterface unit tests */

#include "TunnelInterface.hh"

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/Scheduler/Scheduler.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

namespace {
    void runPPI(ClockService::clock_type t) {
        scheduler::TimerEvent timeout(
                "test-timeout", &scheduler::terminate, scheduler::now() + t);
        ppi::run();
    }
}

SENF_AUTO_TEST_CASE(TunnelInterface)
{
    boost::shared_ptr<emu::InterfaceIdFactoryBase> idFactory (new emu::SimpleInterfaceIdFactory());
    idFactory->getId();

    emu::TunnelServerInterface serverIface (INet4SocketAddress("127.0.0.1:8765"));
    emu::TunnelClientInterface clientIface (INet4SocketAddress("127.0.0.1:5678"));

    clientIface.capacity(emu::tunnel::FromClientToServer, 4242);
    clientIface.capacity(emu::tunnel::FromServerToClient, 1337);
    BOOST_CHECK_EQUAL( clientIface.capacity(emu::tunnel::FromClientToServer), 4242);
    BOOST_CHECK_EQUAL( clientIface.capacity(emu::tunnel::FromServerToClient), 1337);

    BOOST_CHECK( clientIface.canReceive());
    BOOST_CHECK( clientIface.canTransmit());

    serverIface.ifaceIdFactory( idFactory);
    clientIface.ifaceIdFactory( idFactory);

    BOOST_CHECK_EQUAL( serverIface.id(), MACAddress::from_string("02:00:00:00:00:02"));
    BOOST_CHECK_EQUAL( clientIface.id(), MACAddress::from_string("02:00:00:00:00:03"));

    ppi::module::debug::ActiveSource serverSource;
    ppi::module::debug::ActiveSource clientSource;
    ppi::module::debug::PassiveSink serverSink;
    ppi::module::debug::PassiveSink clientSink;

    senf::ppi::connect(serverSource, serverIface);
    senf::ppi::connect(serverIface, serverSink);

    senf::ppi::connect(clientSource, clientIface);
    senf::ppi::connect(clientIface, clientSink);

    clientIface.serverAddress( serverIface.address());
    clientIface.timeout( ClockService::milliseconds(100));
    serverIface.enable();
    clientIface.enable();

    runPPI( ClockService::milliseconds(250));

    BOOST_CHECK( clientIface.established());
    BOOST_CHECK_EQUAL(
            serverIface.capacity(clientIface.id(), emu::tunnel::FromClientToServer),
            clientIface.capacity(emu::tunnel::FromClientToServer));
    BOOST_CHECK_EQUAL(
            serverIface.capacity(clientIface.id(), emu::tunnel::FromServerToClient),
            clientIface.capacity(emu::tunnel::FromServerToClient));

    EthernetPacket p (EthernetPacket::create());
    p->destination() << clientIface.id();
    serverSource.submit( p);

    runPPI( ClockService::milliseconds(250));

    BOOST_CHECK_EQUAL( clientSink.size(), 1);

    serverIface.disable();

    runPPI( ClockService::milliseconds(250));

    BOOST_CHECK(! clientIface.established());

    serverIface.enable();

    runPPI( ClockService::milliseconds(250));

    BOOST_CHECK( clientIface.established());

    p->destination() << serverIface.id();
    clientSource.submit( p);

    runPPI( ClockService::milliseconds(250));

    BOOST_CHECK_EQUAL( serverSink.size(), 1);
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
