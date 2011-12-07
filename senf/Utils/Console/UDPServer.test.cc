// $Id$
//
// Copyright (C) 2009
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
    \brief UDPServer.test unit tests */

//#include "UDPServer.test.hh"
//#include "UDPServer.test.ih"

// Custom includes
#include "Console.hh"
#include <senf/Scheduler/Scheduler.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void timeout()
    {
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

    unsigned nread (0);
    unsigned flags (0);
    std::string data;

    void read(senf::ConnectedUDPv4ClientSocketHandle socket, int ev)
    {
        flags |= ev;
        ++nread;
        data = socket.read();
    }

    senf::ClockService::clock_type delay(unsigned ms)
    {
        return senf::ClockService::now() + senf::ClockService::milliseconds(ms);
    }

}

SENF_AUTO_UNIT_TEST(udpServer)
{
    senf::console::UDPServer server (senf::INet4SocketAddress(localhost4str(0)));
    senf::ConnectedUDPv4ClientSocketHandle socket (senf::INet4SocketAddress(localhost4str(0)));
    senf::scheduler::TimerEvent timer ("udpServer test timer", &timeout);
    senf::scheduler::FdEvent fdev ("udpServer test fd", boost::bind(&read, socket, _1),
                                   socket, senf::scheduler::FdEvent::EV_READ);

    nread = 0;
    flags = 0;
    data = "";
    socket.write(std::string("ll"));
    timer.timeout(delay(300));
    senf::scheduler::process();
    BOOST_CHECK_EQUAL( nread, 1 );
    BOOST_CHECK_EQUAL( flags, senf::scheduler::FdEvent::EV_READ );
    BOOST_CHECK_EQUAL( data, "sys/                        \n" );

    // Check exception handling
    nread = 0;
    flags = 0;
    data = "";
    socket.write(std::string("sys"));
    timer.timeout(delay(300));
    senf::scheduler::process();
    BOOST_CHECK_EQUAL( nread, 1 );
    BOOST_CHECK_EQUAL( flags, senf::scheduler::FdEvent::EV_READ );
    BOOST_CHECK_EQUAL( data, "invalid command\n" "at <unknown>:1:4\n" );

    // switch directory
    nread = 0;
    flags = 0;
    data = "";
    socket.write(std::string("cd sys"));
    timer.timeout(delay(300));
    senf::scheduler::process();
    BOOST_CHECK_EQUAL( nread, 1 );
    BOOST_CHECK_EQUAL( flags, senf::scheduler::FdEvent::EV_READ );
    BOOST_CHECK_EQUAL( data, std::string(1, '\0') );

    // Check that we go back to the root dir for every packet
    nread = 0;
    flags = 0;
    data = "";
    socket.write(std::string("ll"));
    timer.timeout(delay(300));
    senf::scheduler::process();
    BOOST_CHECK_EQUAL( nread, 1 );
    BOOST_CHECK_EQUAL( flags, senf::scheduler::FdEvent::EV_READ );
    BOOST_CHECK_EQUAL( data, "sys/                        \n" );
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
