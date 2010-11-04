// $Id$
//
// Copyright (C) 2009
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
    \brief UDPServer.test unit tests */

//#include "UDPServer.test.hh"
//#include "UDPServer.test.ih"

// Custom includes
#include "Console.hh"

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

    std::string localhost6str(unsigned i)
    {
        return (boost::format("[::1]:%d") % port(i)).str();
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
