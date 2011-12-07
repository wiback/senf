// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief SocketSource unit tests */

//#include "SocketSource.test.hh"
//#include "SocketSource.test.ih"

// Custom includes
#include <algorithm>
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Scheduler/Scheduler.hh>
#include "SocketSource.hh"
#include "DebugModules.hh"
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
                "socketSource test timer", &scheduler::terminate, scheduler::now() + t);
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

}

SENF_AUTO_UNIT_TEST(socketSource)
{
    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress(localhost4str(0)));
    inputSocket.blocking(false);
    module::ActiveSocketSource<> udpSource(inputSocket);
    debug::PassiveSink sink;
    ppi::connect(udpSource, sink);

    std::string data ("TEST");

    senf::UDPv4ClientSocketHandle outputSocket;
    outputSocket.writeto(senf::INet4SocketAddress(localhost4str(0)),data);
    runPPI( senf::ClockService::milliseconds(100));

    BOOST_REQUIRE( ! sink.empty() );
    BOOST_CHECK_EQUAL( sink.front().data().size(), data.size() );
    BOOST_CHECK( std::equal( sink.front().data().begin(), sink.front().data().end(),
                             data.begin()) );
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
