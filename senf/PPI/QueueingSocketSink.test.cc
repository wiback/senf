// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
///////////////////////////////cc.p////////////////////////////////////////
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
    };
}

SENF_AUTO_UNIT_TEST(passiveQueueingSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
            senf::INet4SocketAddress( localhost4str(0)));
    ppi::FIFOQueueingAlgorithm<TestingConnectedDgramWriter::PacketType> queueingAlgorithm ( 100);
    module::PassiveQueueingSocketSink<TestingConnectedDgramWriter> udpSink (
            outputSocket, queueingAlgorithm);
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

    udpSink.writer().throttled = false;

    runPPI( senf::ClockService::milliseconds(200));

    input = inputSocket.read();
    BOOST_CHECK_EQUAL( data, input );
    BOOST_CHECK_EQUAL( udpSink.qAlgorithm().size(), 0);
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
