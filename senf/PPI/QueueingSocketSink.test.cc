// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief QueueingSocketSink unit tests */

#include "QueueingSocketSink.hh"

// Custom includes
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Socket/Protocols/INet/ConnectedUDPSocketHandle.hh>
#include "DebugModules.hh"
#include "SocketSink.hh"
#include "Setup.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace module = ppi::module;
namespace debug = module::debug;
namespace scheduler = senf::scheduler;

namespace {
    void runPPI(senf::ClockService::clock_type t)
    {
        scheduler::TimerEvent timeout(
                "test-timeout", &scheduler::terminate, scheduler::now() + t);
        ppi::run();
    }

    int base_pid = 0;

    unsigned port(unsigned i)
    {
        if (! base_pid)
            base_pid = ::getpid();
        return 23456u + (((base_pid^(base_pid>>8)^(base_pid>>16)^(base_pid>>24))&0xff)<<2) + i;
    }

    std::string localhost4str(unsigned i)
    {
        return (boost::format("localhost:%d") % port(i)).str();
    }

    struct TestingConnectedDgramWriter
        : public ppi::ConnectedDgramWriter
    {
        bool throttled;

        bool operator()(Handle handle, PacketType const & packet)
        {
            if (throttled)
                return false;
            return ConnectedDgramWriter::operator()( handle, packet);
        }

        TestingConnectedDgramWriter(){
            throttled = false;
        }
    };
}

SENF_AUTO_UNIT_TEST(passiveQueueingSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle os (senf::noinit);

    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
            senf::INet4SocketAddress( localhost4str(0)));
    module::PassiveQueueingSocketSink<TestingConnectedDgramWriter> udpSink (
            os, ppi::FIFOQueueingAlgorithm::create());
    
    // test re-assignment of socket
    udpSink.handle( outputSocket);
    
    udpSink.writer().throttled = false;
    debug::ActiveSource source;
    ppi::connect(source, udpSink);
    senf::ppi::init();

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));

    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress(localhost4str(0)));

    source.submit(p);

    std::string input (inputSocket.read());
    BOOST_CHECK_EQUAL( data, input );
    BOOST_CHECK_EQUAL( udpSink.qAlgorithm().size(), 0);

    udpSink.writer().throttled = true;

    source.submit(p);
    BOOST_CHECK_EQUAL( udpSink.qAlgorithm().size(), 1);

    for (int n = 0; n < 100; n++) {
        source.submit(p);
    }
    // queue default size is 64
    BOOST_CHECK_EQUAL( udpSink.qAlgorithm().size(), 64);

    udpSink.writer().throttled = false;

    runPPI( senf::ClockService::milliseconds(250));

    inputSocket.blocking(false);
    while (true) {
        input = inputSocket.read();
        if (input.empty()) break;
        BOOST_CHECK_EQUAL( data, input );
    }

    runPPI( senf::ClockService::milliseconds(250));
    BOOST_CHECK_EQUAL( udpSink.qAlgorithm().size(), 0);
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
