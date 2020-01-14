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
    \brief RawINetSocketHandle unit tests */

#include "RawINetSocketHandle.hh"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "net.test.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf::test;

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

SENF_AUTO_TEST_CASE(RawV4ClientSocketHandle)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::RawV4SocketHandle as non-root user");

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

SENF_AUTO_TEST_CASE(RawV6ClientSocketHandle)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::RawV6SocketHandle as non-root user");

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
