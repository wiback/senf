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
    \brief SocketSink.test unit tests */

//#include "SocketSink.test.hh"
//#include "SocketSink.test.ih"

// Custom includes
#include "../Socket/Protocols/INet/UDPSocketHandle.hh"
#include "../Socket/Protocols/INet/ConnectedUDPSocketHandle.hh"
#include "SocketSource.hh"
#include "DebugModules.hh"
#include "SocketSink.hh"
#include "Setup.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    void timeout() {
        senf::Scheduler::instance().terminate();
    }
}

BOOST_AUTO_UNIT_TEST(passiveSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
        senf::INet4SocketAddress("localhost:44344"));
    module::PassiveSocketSink<> udpSink(outputSocket);
    debug::ActiveSource source;
    ppi::connect(source, udpSink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));

    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress("localhost:44344"));
    senf::ppi::init();
    source.submit(p);

    std::string input (inputSocket.read());
    BOOST_CHECK_EQUAL( data, input );
}

BOOST_AUTO_UNIT_TEST(activeSocketSink)
{
    senf::ConnectedUDPv4ClientSocketHandle outputSocket (
        senf::INet4SocketAddress("localhost:44344"));
    module::ActiveSocketSink<> udpSink(outputSocket);
    debug::PassiveSource source;
    ppi::connect(source, udpSink);

    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));

    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress("localhost:44344"));
    senf::Scheduler::instance().timeout(
        senf::ClockService::now() + senf::ClockService::milliseconds(100), &timeout);
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
