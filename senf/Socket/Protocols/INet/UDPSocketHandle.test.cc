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
    \brief UDPSocketHandle unit tests */

//#include "UDPSocketHandle.test.hh"
//#include "UDPSocketHandle.test.ih"

// Custom includes
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "UDPSocketHandle.hh"
#include "net.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void server_v4()
    {
        int sock = socket(PF_INET,SOCK_DGRAM,0);
        if (sock<0) fail("server_v4","socket()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port(0));
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v4","bind()");

        sin.sin_port = htons(port(1));
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
        sin.sin6_port = htons(port(0));
        sin.sin6_addr = in6addr_loopback;
        if (bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v6","bind()");

        sin.sin6_port = htons(port(1));
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

SENF_AUTO_UNIT_TEST(udpv4ClientSocketHandle)
{
    try {
        alarm(10);
        start(server_v4);
        senf::UDPv4ClientSocketHandle sock;
        SENF_CHECK_NO_THROW( sock.bind(senf::INet4SocketAddress(localhost4str(1))) );
        BOOST_CHECK( sock.local() == senf::INet4SocketAddress(localhost4str(1)) );
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.writeto(senf::INet4SocketAddress(localhost4str(0)),
                                           std::string("TEST-WRITE")) );
        BOOST_CHECK_EQUAL( sock.read(), "TEST-WRITE" );
        SENF_CHECK_NO_THROW( sock.protocol().timestamp() );
        sock.writeto(senf::INet4SocketAddress(localhost4str(0)), std::string("QUIT"));
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
