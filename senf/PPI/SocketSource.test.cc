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
    \brief SocketSource unit tests */

#include "SocketSource.hh"

// Custom includes
#include <algorithm>
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Scheduler/Scheduler.hh>
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(socketSource)
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
