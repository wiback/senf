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
    \brief UDPSocketHandle unit tests */

#include "UDPSocketHandle.hh"

// Custom includes
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "net.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf::test;

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

//    void server_v6()
//    {
//        int sock = socket(PF_INET6,SOCK_DGRAM,0);
//        if (sock<0) fail("server_v6","socket()");
//        struct sockaddr_in6 sin;
//        ::memset(&sin,0,sizeof(sin));
//        sin.sin6_family = AF_INET6;
//        sin.sin6_port = htons(port(0));
//        sin.sin6_addr = in6addr_loopback;
//        if (bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v6","bind()");
//
//        sin.sin6_port = htons(port(1));
//        char buffer[1024];
//        while (1) {
//            int n = read(sock,buffer,1024);
//            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
//                break;
//            sendto(sock,buffer,n,0,(struct sockaddr *)&sin,sizeof(sin));
//        }
//
//        if (close(sock) < 0) fail("server_v6","close()");
//    }

}

SENF_AUTO_TEST_CASE(udpv4ClientSocketHandle)
{
    try {
        alarm(10);
        start(server_v4);
        senf::UDPv4ClientSocketHandle sock;
        SENF_CHECK_NO_THROW( sock.bind(senf::INet4SocketAddress(localhost4str(1))) );
        BOOST_CHECK( sock.local() == senf::INet4SocketAddress(localhost4str(1)) );
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2560 );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2560 );
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
