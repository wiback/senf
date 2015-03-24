// $Id:$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

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
