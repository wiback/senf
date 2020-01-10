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
    \brief UDPServer.test unit tests */

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(udpServer)
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
