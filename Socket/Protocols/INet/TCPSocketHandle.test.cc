// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Unit tests

//#include "TCPSocketHandle.test.hh"
//#include "TCPSocketHandle.test.ih"

// Custom includes
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "TCPSocketHandle.hh"
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
        int serv = socket(PF_INET,SOCK_STREAM,0);
        if (serv<0) fail("server_v4","socket()");
        int v = 1;
        if (setsockopt(serv,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0)
            fail("server_v4","setsockopt()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12345);
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
            write(sock,buffer,n);
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
        sin.sin6_port = htons(12345);
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
            write(sock,buffer,n);
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("server_v6","shutdown()");
        if (close(sock) < 0) fail("server_v6","close()");
        if (close(serv) < 0) fail("server_v6","close()");
    }

}

BOOST_AUTO_UNIT_TEST(tcpv4ClientSocketHandle)
{
    {
        senf::TCPv4ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(senf::INet4SocketAddress("127.0.0.1:12345")), 
                           senf::SystemException );
        BOOST_CHECK_THROW( sock.protocol().connect(senf::INet4SocketAddress("127.0.0.1:12345")),
                           senf::SystemException );
    }

    try {
        alarm(10);
        start(server_v4);
        senf::TCPv4ClientSocketHandle sock;
        BOOST_CHECK_NO_THROW( sock.bind(senf::INet4SocketAddress("127.0.0.1:23456")) );
        BOOST_CHECK_NO_THROW( sock.connect(senf::INet4SocketAddress("127.0.0.1:12345")) );
        BOOST_CHECK( sock.peer() == senf::INet4SocketAddress("127.0.0.1:12345") );
        BOOST_CHECK( sock.local() == senf::INet4SocketAddress("127.0.0.1:23456") );
        BOOST_CHECK( sock.blocking() );
        BOOST_CHECK_NO_THROW( sock.rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.rcvbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.sndbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.write("TEST-WRITE") );
        BOOST_CHECK_EQUAL( sock.read(), "TEST-WRITE" );
        BOOST_CHECK( !sock.eof() );
        sock.write("QUIT");
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
        // realy only be in the UDP Protocol implementation
//        BOOST_CHECK_NO_THROW( sock.protocol().mcTTL() );
//        BOOST_CHECK_THROW( sock.protocol().mcTTL(1), senf::SystemException );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcLoop() );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcLoop(false) );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0") );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0","127.0.0.1:0") );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0","127.0.0.1:0") );
//        BOOST_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0") );
//        BOOST_CHECK_THROW( sock.protocol().mcIface("lo"), senf::SystemException );

        // The following setsockopts are hard to REALLY test ...
        BOOST_CHECK_NO_THROW( sock.protocol().nodelay(true) );
        BOOST_CHECK( sock.protocol().nodelay() );
        BOOST_CHECK_EQUAL( sock.protocol().siocinq(), 0u );
        BOOST_CHECK_EQUAL( sock.protocol().siocoutq(), 0u );

        BOOST_CHECK_NO_THROW( sock.protocol().reuseaddr(true) );
        BOOST_CHECK( sock.protocol().reuseaddr() );
        BOOST_CHECK_NO_THROW( sock.protocol().linger(true,0) );
        BOOST_CHECK( sock.protocol().linger() == std::make_pair(true, 0u) );
    }
}

BOOST_AUTO_UNIT_TEST(tcpv6ClientSocketHandle)
{
    {
        senf::TCPv6ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(senf::INet6SocketAddress("[::1]:12345")), senf::SystemException );
        BOOST_CHECK_THROW( sock.protocol().connect("[::1]:12345"), senf::SystemException );
    }

    try {
        alarm(10);
        start(server_v6);
        senf::TCPv6ClientSocketHandle sock;
        BOOST_CHECK_NO_THROW( sock.bind("[::1]:23456") );
        BOOST_CHECK_NO_THROW( sock.connect("[::1]:12345") );
        BOOST_CHECK( sock.peer() == "[::1]:12345" );
        BOOST_CHECK( sock.local() == "[::1]:23456" );
        BOOST_CHECK( sock.blocking() );
        BOOST_CHECK_NO_THROW( sock.rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.rcvbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.sndbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.write("TEST-WRITE") );
        BOOST_CHECK_EQUAL( sock.read(), "TEST-WRITE" );
        // this fails with ENOFILE ... why ????
        // BOOST_CHECK_NO_THROW( sock.protocol().timestamp() );
        BOOST_CHECK( !sock.eof() );
        sock.write("QUIT");
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
        BOOST_CHECK_NO_THROW( sock.protocol().nodelay(true) );
        BOOST_CHECK( sock.protocol().nodelay() );
        BOOST_CHECK_EQUAL( sock.protocol().siocinq(), 0u );
        BOOST_CHECK_EQUAL( sock.protocol().siocoutq(), 0u );

        BOOST_CHECK_NO_THROW( sock.protocol().reuseaddr(true) );
        BOOST_CHECK( sock.protocol().reuseaddr() );
        BOOST_CHECK_NO_THROW( sock.protocol().linger(true,0) );
        BOOST_CHECK( sock.protocol().linger() == std::make_pair(true, 0u) );
    }
}

///////////////////////////////////////////////////////////////////////////

namespace {

    void client_v4()
    {
        int sock = socket(PF_INET,SOCK_STREAM,0);
        if (sock<0) fail("client_v4","socket()");
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12346);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (connect(sock,(struct sockaddr *)&sin,sizeof(sin)) < 0)
            fail("client_v4","connect()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            write(sock,buffer,n);
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
        sin.sin6_port = htons(12347);
        sin.sin6_addr = in6addr_loopback;
        if (connect(sock,(struct sockaddr *)&sin,sizeof(sin)) < 0)
            fail("client_v6","connect()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            write(sock,buffer,n);
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("client_v6","shutdown()");
        if (close(sock) < 0) fail("client_v6","close()");
    }

}

BOOST_AUTO_UNIT_TEST(tcpv4ServerSocketHandle)
{
    try {
        alarm(10);
        BOOST_CHECKPOINT("Opening server socket");
        senf::TCPv4ServerSocketHandle server (senf::INet4SocketAddress("127.0.0.1:12346"));
        BOOST_CHECKPOINT("Starting client");
        start(client_v4);

        BOOST_CHECKPOINT("Accepting connection");
        senf::TCPv4ClientSocketHandle client = server.accept();
        BOOST_CHECK_NO_THROW(client.write("QUIT"));

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

BOOST_AUTO_UNIT_TEST(tcpv6ServerSocketHandle)
{
    try {
        alarm(10);
        BOOST_CHECKPOINT("Opening server socket");
        senf::TCPv6ServerSocketHandle server ("[::1]:12347");
        BOOST_CHECKPOINT("Starting client");
        start(client_v6);

        BOOST_CHECKPOINT("Accepting connection");
        senf::TCPv6ClientSocketHandle client = server.accept();
        BOOST_CHECK_NO_THROW(client.write("QUIT"));

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
