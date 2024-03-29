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
    \brief FdEvent unit tests */

#include "FdEvent.hh"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <boost/bind.hpp>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    char const * SOCK_PATH = "/tmp/sched_test.sock";

    void error(char const * fn, char const * proc="")
    {
        std::cerr << "\n" << proc << fn << ": " << strerror(errno) << std::endl;
    }

    void fail(char const * fn)
    {
        error(fn,"server: ");
        _exit(1);
    }

    void server()
    {
        int sock = socket(PF_UNIX,SOCK_STREAM,0);
        if (sock<0) fail("socket");
        struct sockaddr_un sun;
        memset(&sun,0,sizeof(sun));
        sun.sun_family = AF_UNIX;
        strcpy(sun.sun_path,SOCK_PATH);
        if (bind(sock,(struct sockaddr*)&sun,sizeof(sun))<0) fail("bind");
        if (listen(sock,1)<0) fail("listen");
        int conn = accept(sock,0,0);
        if (conn < 0) fail("accept");

        //-/////////////////////////////////////////////////////////////////////////////////////////

        if (write(conn,"READ",4)<0) fail("write");
        char buffer[1024];
        int size =  read(conn,buffer,1024);
        if (size<0) fail("read");
        if (size == 5) {
            buffer[5] = 0;
            if (strcmp(buffer,"WRITE")==0) {
                if (write(conn,"OK",2)<0) fail("write");
            } else
                if (write(conn,"FAIL",4)<0) fail("write");
        } else
            if (write(conn,"FAIL",4)<0) fail("write");

        //-/////////////////////////////////////////////////////////////////////////////////////////

        close(conn);
        close(sock);
    }

    int start_server()
    {
        unlink(SOCK_PATH);
        int pid = fork();
        if (pid == 0) {
            signal(SIGCHLD, SIG_IGN);
            server();
            _exit(0);
        }
        if (pid < 0) {
            error("fork");
            return 0;
        }
        signal(SIGCHLD, SIG_DFL);

        sleep(1); // Wait for the server socket to be opened
        return pid;
    }

    bool stop_server(int pid)
    {
        sleep(1); // Wait for the server to terminate
        if (kill(pid,SIGTERM)<0) {
            error("kill");
            return false;
        }
        int status = 0;
        if (waitpid(pid,&status,0)<0) {
            error("waitpid");
            return false;
        }
        unlink(SOCK_PATH);
        if (WIFSIGNALED(status)) {
            std::cerr << "\nserver terminated with signal " << WTERMSIG(status) << std::endl;
            return false;
        }
        if (WEXITSTATUS(status)!=0) {
            std::cerr << "\nserver terminated with exit status " << WEXITSTATUS(status) << std::endl;
            return false;
        }
        return true;
    }

    char buffer[1024];
    int size (0);
    int event (0);
    int calls (0);

    void callback(int fd, int ev)
    {
        ++calls;
        event = ev;
        switch (event & senf::scheduler::FdEvent::EV_ALL) {
        case senf::scheduler::FdEvent::EV_READ:
            size = recv(fd,buffer,1024,0);
            break;
        case senf::scheduler::FdEvent::EV_PRIO:
            size = recv(fd,buffer,1024,MSG_OOB);
            break;
        case senf::scheduler::FdEvent::EV_WRITE:
            size = write(fd,buffer,size);
            break;
        }
    }

}

SENF_AUTO_TEST_CASE(fdDispatcher)
{
    senf::scheduler::detail::FdManager::instance().timeout(1000);

    int pid (start_server());
    BOOST_REQUIRE( pid );

    int sock = socket(PF_UNIX,SOCK_STREAM,0);
    if (sock<0) {
        error("socket");
        BOOST_FAIL("socket");
    }
    struct sockaddr_un sun;
    memset(&sun,0,sizeof(sun));
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path,SOCK_PATH);

    if (connect(sock,(struct sockaddr*)&sun,sizeof(sun))<0) {
        error("connect");
        BOOST_FAIL("connect");
    }

    {
        senf::scheduler::FdEvent sockread ("testHandler",  boost::bind(&callback, sock, _1),
                                           sock, senf::scheduler::FdEvent::EV_READ);
        senf::scheduler::FdEvent sockwrite ("testHandler", boost::bind(&callback, sock, _1),
                                            sock, senf::scheduler::FdEvent::EV_WRITE, false);
        event = 0;
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_READ );
        BOOST_CHECK_EQUAL( size, 4 );
        buffer[size] = 0;
        BOOST_CHECK_EQUAL( buffer, "READ" );

        strcpy(buffer,"WRITE");
        size=5;
        SENF_CHECK_NO_THROW( sockwrite.enable() );
        event = 0;
        sleep(1);
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_WRITE );

        SENF_CHECK_NO_THROW( sockwrite.disable() );
        event = 0;
        sleep(1);
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_HUP | senf::scheduler::FdEvent::EV_READ );
        BOOST_CHECK_EQUAL( size, 2 );
        buffer[size]=0;
        BOOST_CHECK_EQUAL( buffer, "OK" );

        BOOST_CHECK_EQUAL( calls, 3 );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        BOOST_CHECK_EQUAL( calls, 3 );

        // Ensure, removing an already closed file-descriptor doesn't wreak havoc
        close(sock);
    }

    SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
    BOOST_CHECK_EQUAL( calls, 3 );

    BOOST_CHECK (stop_server(pid));
}

namespace {

    bool is_close(senf::ClockService::clock_type a, senf::ClockService::clock_type b)
    {
        return (a<b ? b-a : a-b) < senf::ClockService::milliseconds(50);
    }

    bool called (false);
    void handler(int events)
    {
        called=true;
    }
}

SENF_AUTO_TEST_CASE(fileDispatcher)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");

    senf::scheduler::detail::FileDispatcher::instance().timeout(500);

    int fd (open("test.empty.file", O_RDWR|O_CREAT|O_TRUNC, 0600));

    senf::ClockService::clock_type t (senf::ClockService::now());
    try {
        senf::scheduler::FdEvent fde ("testHandler", &handler,
                                      fd, senf::scheduler::FdEvent::EV_READ);
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FileDispatcher::instance().prepareRun() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );

        BOOST_CHECK( called );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (t)(senf::ClockService::now()) );
    }
    catch (std::exception const & ex) {
        std::cerr << "Exception:\n" << ex.what() << "\n";
        throw;
    }
    close(fd);

    called = false;
    t = senf::ClockService::now();
    SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
    SENF_CHECK_NO_THROW( senf::scheduler::detail::FileDispatcher::instance().prepareRun() );
    SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );

    BOOST_CHECK( ! called );
    if (enabled)
        BOOST_CHECK_PREDICATE(
            is_close, (t+senf::ClockService::milliseconds(500))(senf::ClockService::now()) );

    unlink("test.empty.file");
    senf::scheduler::detail::FileDispatcher::instance().timeout(-1);
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
