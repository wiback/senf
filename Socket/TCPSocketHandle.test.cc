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
        std::cerr << "\n" << proc << fn << ": " << strerror(errno) << std::endl;
    }

    void fail(char const * fn)
    {
        error(fn,"server: ");
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
        kill(server_pid,9);
        wait();
    }

}

///////////////////////////////////////////////////////////////////////////

namespace {

    void server()
    {
        int serv = socket(PF_INET,SOCK_STREAM,0);
        if (serv<0) fail("socket()");
        int v = 1;
        if (setsockopt(serv,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0)
            fail("setsockopt()");
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12345);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(serv,(struct sockaddr *)&sin,sizeof(sin))<0) fail("bind()");
        if (listen(serv,1)<0) fail("listen()");
        int sock = accept(serv,0,0);
        if (sock < 0) fail("accept()");

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            write(sock,buffer,n);
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("shutdown()");
        if (close(sock) < 0) fail("close()");
        if (close(serv) < 0) fail("close()");
    }

}

BOOST_AUTO_UNIT_TEST(tcpv4ClientSocketHandle)
{
    {
        senf::TCPv4ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(senf::INet4Address("127.0.0.1:12345")), senf::SystemException );
        BOOST_CHECK_THROW( sock.protocol().connect("127.0.0.1:12345"), senf::SystemException );
    }

    {
        start(server);
        senf::TCPv4ClientSocketHandle sock;
        BOOST_CHECK_NO_THROW( sock.bind("127.0.0.1:23456") );
        BOOST_CHECK_NO_THROW( sock.connect("127.0.0.1:12345") );
        BOOST_CHECK( sock.peer() == "127.0.0.1:12345" );
        BOOST_CHECK( sock.local() == "127.0.0.1:23456" );
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
    }
    
    {
        senf::TCPv4ClientSocketHandle sock;

        // Since this is a TCP socket, most of the calls will fail or
        // are at least not sensible ...
        // I'll have to move those to a UDPSocket test ... they should
        // realy only be in the UDP Protocol implementation
        BOOST_CHECK_NO_THROW( sock.protocol().mcTTL() );
        BOOST_CHECK_THROW( sock.protocol().mcTTL(1), senf::SystemException );
        BOOST_CHECK_NO_THROW( sock.protocol().mcLoop() );
        BOOST_CHECK_NO_THROW( sock.protocol().mcLoop(false) );
        BOOST_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0") );
        BOOST_CHECK_NO_THROW( sock.protocol().mcAddMembership("224.0.0.1:0","127.0.0.1:0") );
        BOOST_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0","127.0.0.1:0") );
        BOOST_CHECK_NO_THROW( sock.protocol().mcDropMembership("224.0.0.1:0") );
        BOOST_CHECK_THROW( sock.protocol().mcIface("lo"), senf::SystemException );
        
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

    void client()
    {
        int sock = socket(PF_INET,SOCK_STREAM,0);
        if (sock<0) fail("socket()");
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12346);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (connect(sock,(struct sockaddr *)&sin,sizeof(sin)) < 0)
            fail("connect()");
        
        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            write(sock,buffer,n);
        }

        if (shutdown(sock, SHUT_RDWR) < 0) fail("shutdown()");
        if (close(sock) < 0) fail("close()");
    }

}

BOOST_AUTO_UNIT_TEST(tcpv4ServerSocketHandle)
{
    {
        BOOST_CHECKPOINT("Opening server socket");
        senf::TCPv4ServerSocketHandle server ("127.0.0.1:12346");
        BOOST_CHECKPOINT("Starting client");
        start(client);

        BOOST_CHECKPOINT("Accepting connection");
        senf::TCPv4ClientSocketHandle client = server.accept();
        BOOST_CHECK_NO_THROW(client.write("QUIT"));

        BOOST_CHECKPOINT("Stopping client");
        sleep(1);
        stop();
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
