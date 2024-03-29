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
    \brief TCPSocketHandle unit tests */

#include "TCPSocketHandle.hh"

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
        int serv = socket(PF_INET,SOCK_STREAM,0);
        if (serv<0) fail("server_v4","socket()");
        int v = 1;
        if (setsockopt(serv,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0)
            fail("server_v4","setsockopt()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port(0));
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(serv,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v4","bind()");
        if (listen(serv,1)<0) fail("server_v4","listen()");
        int sock = accept(serv,0,0);
        if (sock < 0) fail("server_v4","accept()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            senf::IGNORE( write(sock,buffer,n) );
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("server_v4","shutdown()");
        if (close(sock) < 0) fail("server_v4","close()");
        if (close(serv) < 0) fail("server_v4","close()");
    }

    void server_v6()
    {
        int serv = socket(PF_INET6,SOCK_STREAM,0);
        if (serv<0) fail("server_v6","socket()");
        int v = 1;
        if (setsockopt(serv,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0)
            fail("server_v6","setsockopt()");
        struct sockaddr_in6 sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(port(0));
        sin.sin6_addr = in6addr_loopback;
        if (bind(serv,(struct sockaddr *)&sin,sizeof(sin))<0) fail("server_v6","bind()");
        if (listen(serv,1)<0) fail("server_v6","listen()");
        int sock = accept(serv,0,0);
        if (sock < 0) fail("server_v6","accept()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            senf::IGNORE( write(sock,buffer,n) );
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("server_v6","shutdown()");
        if (close(sock) < 0) fail("server_v6","close()");
        if (close(serv) < 0) fail("server_v6","close()");
    }

}

SENF_AUTO_TEST_CASE(tcpv4ClientSocketHandle)
{
    {
        senf::TCPv4ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(senf::INet4SocketAddress(localhost4str(0))),
                           senf::SystemException );
    }

    try {
        alarm(10);
        start(server_v4);
        senf::TCPv4ClientSocketHandle sock;
        SENF_CHECK_NO_THROW( sock.bind(senf::INet4SocketAddress(localhost4str(1))) );
        SENF_CHECK_NO_THROW( sock.connect(senf::INet4SocketAddress(localhost4str(0))) );
        BOOST_CHECK( sock.peer() == senf::INet4SocketAddress(localhost4str(0)) );
        BOOST_CHECK( sock.local() == senf::INet4SocketAddress(localhost4str(1)) );
        BOOST_CHECK( sock.blocking() );
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2560 );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2560 );
        SENF_CHECK_NO_THROW( sock.write(std::string("TEST-WRITE")) );
        BOOST_CHECK_EQUAL( sock.read(), std::string("TEST-WRITE") );
        BOOST_CHECK( !sock.eof() );
        sock.write(std::string("QUIT"));
        sleep(1);
        stop();
        sleep(1);
        BOOST_CHECK_EQUAL( sock.read(), "" );
        BOOST_CHECK( sock.eof() );
        BOOST_CHECK( !sock );
        alarm(0);
    } catch (...) {
        alarm(0);
        sleep(1);
        stop();
        sleep(1);
        throw;
    }

    {
        senf::TCPv4ClientSocketHandle sock;

        // Since this is a TCP socket, most of the calls will fail or
        // are at least not sensible ...
        // I'll have to move those to a UDPSocket test ... they should
        // really only be in the UDP Protocol implementation
//        SENF_CHECK_NO_THROW( sock.protocol().mcTTL() );
//        BOOST_CHECK_THROW( sock.protocol().mcTTL(1), senf::SystemException );
//        SENF_CHECK_NO_THROW( sock.protocol().mcLoop() );
//        SENF_CHECK_NO_THROW( sock.protocol().mcLoop(false) );
//        SENF_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0") );
//        SENF_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0","127.0.0.1:0") );
//        SENF_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0","127.0.0.1:0") );
//        SENF_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0") );
//        BOOST_CHECK_THROW( sock.protocol().mcIface("lo"), senf::SystemException );

        // The following setsockopts are hard to REALLY test ...
        SENF_CHECK_NO_THROW( sock.protocol().nodelay(true) );
        BOOST_CHECK( sock.protocol().nodelay() );
        BOOST_CHECK_EQUAL( sock.protocol().siocinq(), 0u );
        BOOST_CHECK_EQUAL( sock.protocol().siocoutq(), 0u );

        SENF_CHECK_NO_THROW( sock.protocol().reuseaddr(true) );
        BOOST_CHECK( sock.protocol().reuseaddr() );
        SENF_CHECK_NO_THROW( sock.protocol().linger(true,0) );
        BOOST_CHECK( sock.protocol().linger() == std::make_pair(true, 0u) );
    }
}

SENF_AUTO_TEST_CASE(tcpv6ClientSocketHandle)
{
    {
        senf::TCPv6ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(senf::INet6SocketAddress(localhost6str(0))),
                           senf::SystemException );
    }

    try {
        alarm(10);
        start(server_v6);
        senf::TCPv6ClientSocketHandle sock;
        SENF_CHECK_NO_THROW( sock.bind(senf::INet6SocketAddress(localhost6str(1))) );
        SENF_CHECK_NO_THROW( sock.connect(senf::INet6SocketAddress(localhost6str(0))) );
        BOOST_CHECK( sock.peer() == senf::INet6SocketAddress(localhost6str(0)) );
        BOOST_CHECK( sock.local() == senf::INet6SocketAddress(localhost6str(1)) );
        BOOST_CHECK( sock.blocking() );
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2560 );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2560) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2560 );
        SENF_CHECK_NO_THROW( sock.write(std::string("TEST-WRITE")) );
        BOOST_CHECK_EQUAL( sock.read(), std::string("TEST-WRITE") );
        // this fails with ENOFILE ... why ????
        // SENF_CHECK_NO_THROW( sock.protocol().timestamp() );
        BOOST_CHECK( !sock.eof() );
        sock.write(std::string("QUIT"));
        sleep(1);
        stop();
        sleep(1);
        BOOST_CHECK_EQUAL( sock.read(), "" );
        BOOST_CHECK( sock.eof() );
        BOOST_CHECK( !sock );
        alarm(0);
    } catch (...) {
        alarm(0);
        sleep(1);
        stop();
        sleep(1);
        throw;
    }

    {
        senf::TCPv6ClientSocketHandle sock;

        // The following setsockopts are hard to REALLY test ...
        SENF_CHECK_NO_THROW( sock.protocol().nodelay(true) );
        BOOST_CHECK( sock.protocol().nodelay() );
        BOOST_CHECK_EQUAL( sock.protocol().siocinq(), 0u );
        BOOST_CHECK_EQUAL( sock.protocol().siocoutq(), 0u );

        SENF_CHECK_NO_THROW( sock.protocol().reuseaddr(true) );
        BOOST_CHECK( sock.protocol().reuseaddr() );
        SENF_CHECK_NO_THROW( sock.protocol().linger(true,0) );
        BOOST_CHECK( sock.protocol().linger() == std::make_pair(true, 0u) );
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void client_v4()
    {
        int sock = socket(PF_INET,SOCK_STREAM,0);
        if (sock<0) fail("client_v4","socket()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port(2));
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (connect(sock,(struct sockaddr *)&sin,sizeof(sin)) < 0)
            fail("client_v4","connect()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            senf::IGNORE( write(sock,buffer,n) );
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("client_v4","shutdown()");
        if (close(sock) < 0) fail("client_v4","close()");
    }

    void client_v6()
    {
        int sock = socket(PF_INET6,SOCK_STREAM,0);
        if (sock<0) fail("client_v6","socket()");
        struct sockaddr_in6 sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(port(3));
        sin.sin6_addr = in6addr_loopback;
        if (connect(sock,(struct sockaddr *)&sin,sizeof(sin)) < 0)
            fail("client_v6","connect()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            senf::IGNORE( write(sock,buffer,n) );
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("client_v6","shutdown()");
        if (close(sock) < 0) fail("client_v6","close()");
    }

}

SENF_AUTO_TEST_CASE(tcpv4ServerSocketHandle)
{
    try {
        alarm(10);
        BOOST_CHECKPOINT("Opening server socket");
        senf::TCPv4ServerSocketHandle server (senf::INet4SocketAddress(localhost4str(2)));
        BOOST_CHECKPOINT("Starting client");
        start(client_v4);

        BOOST_CHECKPOINT("Accepting connection");
        senf::TCPv4ClientSocketHandle client = server.accept();
        SENF_CHECK_NO_THROW(client.write(std::string("QUIT")));

        BOOST_CHECKPOINT("Stopping client");
        sleep(1);
        stop();
        alarm(0);
    } catch (...) {
        alarm(0);
        sleep(1);
        stop();
        sleep(1);
        throw;
    }
}

SENF_AUTO_TEST_CASE(tcpv6ServerSocketHandle)
{
    try {
        alarm(10);
        BOOST_CHECKPOINT("Opening server socket");
        senf::TCPv6ServerSocketHandle server (senf::INet6SocketAddress(localhost6str(3)));
        BOOST_CHECKPOINT("Starting client");
        start(client_v6);

        BOOST_CHECKPOINT("Accepting connection");
        senf::TCPv6ClientSocketHandle client = server.accept();
        SENF_CHECK_NO_THROW(client.write(std::string("QUIT")));

        BOOST_CHECKPOINT("Stopping client");
        sleep(1);
        stop();
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
