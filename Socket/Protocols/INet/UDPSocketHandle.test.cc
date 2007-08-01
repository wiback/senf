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
    \brief UDPSocketHandle.test unit tests */

//#include "UDPSocketHandle.test.hh"
//#include "UDPSocketHandle.test.ih"

// Custom includes
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "UDPSocketHandle.hh"
#include <iostream>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    void error(char const * fn, char const * proc="")
    {
        std::cerr << "\n" << proc << ((*proc)?": ":"") << fn << ": " << strerror(errno) << std::endl;
    }

    void fail(char const * proc, char const * fn)
    {
        error(fn,proc);
        _exit(1);
    }

    int server_pid = 0;

    void start(void (*fn)())
    {
        server_pid = ::fork();
        if (server_pid < 0) BOOST_FAIL("fork()");
        if (server_pid == 0) {
            (*fn)();
            _exit(0);
        }
    }

    void wait()
    {
        int status;
        if (waitpid(server_pid,&status,0)<0)
            BOOST_FAIL("waitpid()");
        BOOST_CHECK_EQUAL( status , 0 );
    }

    void stop()
    {
        if (server_pid) {
            kill(server_pid,9);
            wait();
            server_pid = 0;
        }
    }

}

///////////////////////////////////////////////////////////////////////////

namespace {

    void server_v4()
    {
        int sock = socket(PF_INET,SOCK_DGRAM,0);
        if (sock<0) fail("server_v4","socket()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12345);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v4","bind()");

        sin.sin_port = htons(23456);
        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            sendto(sock,buffer,n,0,(struct sockaddr *)&sin,sizeof(sin));
        }

        if (close(sock) < 0) fail("server_v4","close()");
    }

    void server_v6()
    {
        int sock = socket(PF_INET6,SOCK_DGRAM,0);
        if (sock<0) fail("server_v6","socket()");
        struct sockaddr_in6 sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(12345);
        sin.sin6_addr = in6addr_loopback;
        if (bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v6","bind()");

        sin.sin6_port = htons(23456);
        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            sendto(sock,buffer,n,0,(struct sockaddr *)&sin,sizeof(sin));
        }

        if (close(sock) < 0) fail("server_v6","close()");
    }

}

BOOST_AUTO_UNIT_TEST(udpv4ClientSocketHandle)
{
    try {
        alarm(10);
        start(server_v4);
        senf::UDPv4ClientSocketHandle sock;
        BOOST_CHECK_NO_THROW( sock.bind(senf::INet4SocketAddress("127.0.0.1:23456")) );
        BOOST_CHECK( sock.local() == senf::INet4SocketAddress("127.0.0.1:23456") );
        BOOST_CHECK_NO_THROW( sock.rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.rcvbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.sndbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.writeto(senf::INet4SocketAddress("127.0.0.1:12345"),
                                           std::string("TEST-WRITE")) );
        BOOST_CHECK_EQUAL( sock.read(), "TEST-WRITE" );
        BOOST_CHECK_NO_THROW( sock.protocol().timestamp() );
        sock.writeto(senf::INet4SocketAddress("127.0.0.1:12345"),"QUIT");
        sleep(1);
        stop();
        sleep(1);
        alarm(0);
    } catch (...) {
        alarm(0);
        sleep(1);
        stop();
        sleep(1);
        throw;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
