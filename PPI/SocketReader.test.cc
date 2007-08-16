// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief SocketReader.test unit tests */

//#include "SocketReader.test.hh"
//#include "SocketReader.test.ih"

// Custom includes
#include <algorithm>
#include "Socket/Protocols/INet/UDPSocketHandle.hh"
#include "Scheduler/Scheduler.hh"
#include "SocketReader.hh"
#include "DebugModules.hh"
#include "Setup.hh"

#include <boost/test/auto_unit_test.hpp>
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

BOOST_AUTO_UNIT_TEST(socketReader)
{
    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress("localhost:44344"));
    inputSocket.blocking(false);
    module::ActiveSocketReader<> udpReader(inputSocket);
    debug::PassivePacketSink sink;
    ppi::connect(udpReader.output, sink.input);

    std::string data ("TEST");

    senf::UDPv4ClientSocketHandle outputSocket;
    outputSocket.writeto(senf::INet4SocketAddress("localhost:44344"),data);
    senf::Scheduler::instance().timeout(1000, &timeout);
    senf::ppi::run();

    BOOST_REQUIRE( ! sink.empty() );
    BOOST_CHECK_EQUAL( sink.front().data().size(), data.size() );
    BOOST_CHECK( std::equal( sink.front().data().begin(), sink.front().data().end(), 
                             data.begin()) );
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
