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
