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
    \brief SocketSink unit tests */

#include "SocketSink.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Socket/Protocols/INet/ConnectedUDPSocketHandle.hh>
#include "SocketSource.hh"
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    void timeout() {
        senf::scheduler::terminate();
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

}

SENF_AUTO_TEST_CASE(passiveSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
        senf::INet4SocketAddress(localhost4str(0)));
    module::PassiveSocketSink<> udpSink(outputSocket);
    debug::ActiveSource source;
    ppi::connect(source, udpSink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));

    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress(localhost4str(0)));
    senf::ppi::init();
    source.submit(p);

    std::string input (inputSocket.read());
    BOOST_CHECK_EQUAL( data, input );
}

SENF_AUTO_TEST_CASE(activeSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
        senf::INet4SocketAddress(localhost4str(0)));
    module::ActiveSocketSink<> udpSink(outputSocket);
    debug::PassiveSource source;
    ppi::connect(source, udpSink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));

    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress(localhost4str(0)));
    senf::scheduler::TimerEvent timer (
        "activeSocketSink test timer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(100));
    source.submit(p);
    senf::ppi::run();

    std::string input (inputSocket.read());
    BOOST_CHECK_EQUAL( data, input );
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
