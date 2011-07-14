// $Id: RawINetSocketHandle.test.cc 597 2008-01-15 09:16:20Z g0dil $
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
//   David Wagner <dw6@berlios.de>
/** \file
    \brief RawINetSocketHandle unit tests */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "RawINetSocketHandle.hh"
#include <iostream>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void server_v4() //listen for packets with proto=47 (GRE) and resend them with proto=48
    {
        struct sockaddr_in sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(0);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        int sockrec = socket(PF_INET,SOCK_RAW,47);
        if (sockrec<0) fail("server_v4","socket(rec)");
        int socksend = socket(PF_INET,SOCK_RAW,48);
        if (socksend<0) fail("server_v4","socket(send)");

        char buffer[1024];
        while (1) {
            int n = read(sockrec,buffer,1024);
            if (n == 20+4 )//&& strncmp(,"QUIT",4) == 0)
                break;
            sleep(1);
            //jaja, fieses gehacke...
            sendto(socksend,buffer+20,n-20,0,(struct sockaddr *)&sin,sizeof(sin));
        }

        if (close(sockrec) < 0) fail("server_v4","close(rec)");
        if (close(socksend) < 0) fail("server_v4","close(send)");
    }
    void server_v6() //listen for packets with proto=47 (GRE) and resend them with proto=48
    {
        struct sockaddr_in6 sin;
        ::memset(&sin,0,sizeof(sin));
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(0);
        inet_pton(AF_INET6, "::1", &sin.sin6_addr);
        int sockrec6 = socket(PF_INET6,SOCK_RAW,47);
        if (sockrec6<0) fail("server_v6","socket(rec)");
        int socksend6 = socket(PF_INET6,SOCK_RAW,48);
        if (socksend6<0) fail("server_v6","socket(send)");
        char buffer[1024];
        while (1) {
            int n = read(sockrec6,buffer,1024);
            if (n<0) fail("server_v6","read(sockrec6)");
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            sleep(1);
            //jaja, fieses gehacke...
            n = sendto(socksend6,buffer,n,0,(struct sockaddr *)&sin,sizeof(sin));
            if (n<0) fail("server_v6","sendto(socksend6)");
        }
        if (close(sockrec6) < 0) fail("server_v6","close(rec)");
        if (close(socksend6) < 0) fail("server_v6","close(send)");
    }

}

SENF_AUTO_UNIT_TEST(RawV4ClientSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::RawV4SocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }
    try {
        std::string test = "TEST-WRITE";
        alarm(10);
        start(server_v4);
        senf::RawV4ClientSocketHandle sock(47);  //IPPROTO_GRE
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.writeto(senf::INet4SocketAddress("127.0.0.1:0"), test) );
        senf::RawV4ClientSocketHandle sockrec(48);  //IPPROTO_GRE+1
        std::string in = sockrec.read();
        BOOST_CHECK_EQUAL(in.substr(20), test);
        SENF_CHECK_NO_THROW( sock.writeto(senf::INet4SocketAddress("127.0.0.1:0"),"QUIT"));
        //sock.close();
        //sockrec.close();
        alarm(0);
    } catch (...) {
        alarm(0);
        throw;
    }
}

SENF_AUTO_UNIT_TEST(RawV6ClientSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::RawV6SocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }
    try {
        std::string test = "TEST-WRITE";
        alarm(5);
        start(server_v6);
        sleep(1);
        senf::RawV6ClientSocketHandle sock(47);  //IPPROTO_GRE
        SENF_CHECK_NO_THROW( sock.protocol().rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().rcvbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.protocol().sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.protocol().sndbuf(), 2048u );
        SENF_CHECK_NO_THROW( sock.writeto(senf::INet6SocketAddress("[::1]:0"), test) );
        senf::RawV6ClientSocketHandle sockrec(48);  //IPPROTO_GRE+1
        std::string in = sockrec.read();
        BOOST_CHECK_EQUAL(in, test);
        SENF_CHECK_NO_THROW( sock.writeto(senf::INet6SocketAddress("[::1]:0"),"QUIT"));
        alarm(0);
    } catch (...) {
        alarm(0);
        throw;
    }
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
