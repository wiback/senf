// $Id$
//
// Copyright (C) 2006
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
    \brief Scheduler unit tests */

//#include "Scheduler.test.hh"
//#include "Scheduler.test.ih"

// Custom includes
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <boost/bind.hpp>

#include "Scheduler.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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
    int size;
    int event;

    void callback(int fd, int ev)
    {
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
        senf::scheduler::terminate();
    }

    bool timeoutCalled = false;
    void timeout()
    {
        timeoutCalled = true;
        senf::scheduler::terminate();
    }

    struct HandleWrapper
    {
        HandleWrapper(int fd,std::string const & tag) : fd_(fd), tag_(tag) {}
        int fd_;
        std::string tag_;
    };

    int retrieve_filehandle(HandleWrapper const & handle)
    {
        return handle.fd_;
    }

    void handleCallback(HandleWrapper const & handle, int event)
    {
        if (handle.tag_ != "TheTag")
            return;
        callback(handle.fd_,event);
    }

    bool is_close(senf::ClockService::clock_type a, senf::ClockService::clock_type b)
    {
        return (a<b ? b-a : a-b) < senf::ClockService::milliseconds(100);
    }

    senf::ClockService::clock_type sigtime (0);

    void sigusr(siginfo_t const &)
    {
        sigtime = senf::ClockService::now();
        senf::scheduler::terminate();
    }

    void delay(unsigned long milliseconds)
    {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        while (nanosleep(&ts,&ts) < 0 && errno == EINTR) ;
    }

    void blockingHandler()
    {
        delay(2200);
        senf::scheduler::terminate();
    }

    unsigned eventCount (0);

    void eventeventhandler()
    {
        ++ eventCount;
    }
}

void schedulerTest()
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");

    int pid = start_server();
    BOOST_REQUIRE (pid);

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

    //-/////////////////////////////////////////////////////////////////////////////////////////////

    senf::scheduler::EventHook evev ("eventCounter", eventeventhandler,
                                     senf::scheduler::EventHook::PRE);

    {
        senf::scheduler::FdEvent fde1 ("testFdEvent", boost::bind(&callback, sock, _1),
                                      sock, senf::scheduler::FdEvent::EV_READ);
        event = senf::scheduler::FdEvent::EV_NULL;
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_READ );
        BOOST_REQUIRE_EQUAL( size, 4 );
        buffer[size]=0;
        BOOST_CHECK_EQUAL( buffer, "READ" );

        HandleWrapper handle(sock,"TheTag");
        senf::scheduler::FdEvent fde2 ("testFdEvent", boost::bind(&handleCallback,handle,_1),
                                      handle, senf::scheduler::FdEvent::EV_WRITE);
        strcpy(buffer,"WRITE");
        size=5;
        event = senf::scheduler::FdEvent::EV_NULL;
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_WRITE );

        SENF_CHECK_NO_THROW( fde2.disable() );
        event = senf::scheduler::FdEvent::EV_NULL;
        sleep(1);
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_READ|senf::scheduler::FdEvent::EV_HUP );
        BOOST_REQUIRE_EQUAL( size, 2 );
        buffer[size]=0;
        BOOST_CHECK_EQUAL( buffer, "OK" );
    }

    {
        senf::scheduler::TimerEvent timer1 ("testTimer1", &timeout,
                                            senf::ClockService::now()+senf::ClockService::milliseconds(200));
        senf::scheduler::TimerEvent timer2 ("testTimer2", &timeout,
                                            senf::ClockService::now()+senf::ClockService::milliseconds(400));

        event = senf::scheduler::FdEvent::EV_NULL;
        senf::ClockService::clock_type t (senf::ClockService::now());
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (senf::ClockService::now()-t) (senf::ClockService::milliseconds(200)) );
        BOOST_CHECK( timeoutCalled );
        BOOST_CHECK( ! timer1.enabled() );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_NULL );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (senf::ClockService::now()) (senf::scheduler::eventTime()) );
        timeoutCalled = false;
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (senf::ClockService::now()-t) (senf::ClockService::milliseconds(400)) );
        BOOST_CHECK( timeoutCalled );
        BOOST_CHECK_EQUAL( event, senf::scheduler::FdEvent::EV_NULL );
        BOOST_CHECK( ! timer2.enabled() );

        BOOST_MESSAGE( "A 'Scheduler task hanging' error is expected to be signaled here." );
        SENF_CHECK_NO_THROW( timer1.action(&blockingHandler) );
        SENF_CHECK_NO_THROW( timer1.timeout(senf::ClockService::now()) );
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        BOOST_CHECK_EQUAL( senf::scheduler::watchdogEvents(), 1u );
    }

    {
        senf::scheduler::TimerEvent timer ("testWatchdog", &timeout,
                                           senf::ClockService::now()+senf::ClockService::milliseconds(400));
        senf::scheduler::SignalEvent sig (SIGUSR1, &sigusr);

        senf::ClockService::clock_type t = senf::ClockService::now();
        ::kill(::getpid(), SIGUSR1);
        delay(200);
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        if (enabled) {
            BOOST_CHECK_PREDICATE( is_close, (senf::ClockService::now()) (t+senf::ClockService::milliseconds(200)) );
            BOOST_CHECK_PREDICATE( is_close, (sigtime) (t+senf::ClockService::milliseconds(200)) );
        }
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
    }

    BOOST_CHECK( eventCount >= 8u );


    //-/////////////////////////////////////////////////////////////////////////////////////////////

    close(sock);

    BOOST_CHECK (stop_server(pid));
}

SENF_AUTO_UNIT_TEST(testSchedulerPollTimers)
{
    BOOST_CHECK( ! senf::scheduler::usingHiresTimers() );
    schedulerTest();
}

SENF_AUTO_UNIT_TEST(testSchedulerHiresTimers)
{
    if (senf::scheduler::haveScalableHiresTimers())
        BOOST_MESSAGE( "Using timerfd() hires timers" );
    else
        BOOST_MESSAGE( "Using POSIX hires timers");
    SENF_CHECK_NO_THROW( senf::scheduler::hiresTimers() );
    BOOST_CHECK( senf::scheduler::usingHiresTimers() );
    schedulerTest();
    SENF_CHECK_NO_THROW( senf::scheduler::loresTimers() );
    BOOST_CHECK( ! senf::scheduler::usingHiresTimers() );
}

SENF_AUTO_UNIT_TEST(testSchedulerPOSIXTimers)
{
    if (senf::scheduler::haveScalableHiresTimers()) {
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().timerSource(
                                 std::auto_ptr<senf::scheduler::detail::TimerSource>(
                                     new senf::scheduler::detail::POSIXTimerSource())) );
        schedulerTest();
        senf::scheduler::loresTimers();
    }
}

namespace {

    void sigme()
    {
        senf::scheduler::BlockSignals signalBlocker;
        signalBlocker.block();
        signalBlocker.unblock();
        BOOST_CHECK( ! signalBlocker.blocked() );
        signalBlocker.unblock();
        signalBlocker.block();
        BOOST_CHECK( signalBlocker.blocked() );
        ::kill(::getpid(), SIGUSR1);
        delay(200);
    }

}

SENF_AUTO_UNIT_TEST(blockSignals)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");

    senf::scheduler::TimerEvent signaler ("sigme", &sigme, senf::ClockService::now());
    senf::scheduler::TimerEvent timer (
        "testWatchdog", &timeout, senf::ClockService::now()+senf::ClockService::milliseconds(400));
    senf::scheduler::SignalEvent sig (SIGUSR1, &sigusr);

    senf::ClockService::clock_type t = senf::ClockService::now();
    SENF_CHECK_NO_THROW( senf::scheduler::process() );

    if (enabled) {
        BOOST_CHECK_PREDICATE( is_close,
                               (senf::ClockService::now())
                               (t+senf::ClockService::milliseconds(200)) );
        BOOST_CHECK_PREDICATE( is_close, (sigtime) (t+senf::ClockService::milliseconds(200)) );
    }

    SENF_CHECK_NO_THROW( senf::scheduler::process() );
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
