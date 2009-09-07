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
    \brief SocketSink unit tests */

//#include "SocketSink.test.hh"
//#include "SocketSink.test.ih"

// Custom includes
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Socket/Protocols/INet/ConnectedUDPSocketHandle.hh>
#include "SocketSource.hh"
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

    std::string localhost6str(unsigned i)
    {
        return (boost::format("[::1]:%d") % port(i)).str();
    }
}

BOOST_AUTO_UNIT_TEST(passiveSocketSink)
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

BOOST_AUTO_UNIT_TEST(activeSocketSink)
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
