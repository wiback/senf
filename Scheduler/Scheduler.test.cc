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

//#include "scheduler.test.hh"
//#include "scheduler.test.ih"

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

#include "Scheduler.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::lib;

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

        ///////////////////////////////////////////////////////////////////////////

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

        ///////////////////////////////////////////////////////////////////////////

        close(conn);
        close(sock);
    }

    int start_server()
    {
        unlink(SOCK_PATH);
        int pid = fork();
        if (pid == 0) {
            server();
            _exit(0);
        }
        if (pid < 0) {
            error("fork");
            return 0;
        }
            
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

    void callback(int fd, Scheduler::EventId ev)
    {
        event = ev;
        switch (event) {
        case Scheduler::EV_READ:
            size = recv(fd,buffer,1024,0);
            break;
        case Scheduler::EV_PRIO:
            size = recv(fd,buffer,1024,MSG_OOB);
            Scheduler::instance().terminate();
            break;
        case Scheduler::EV_WRITE:
            size = write(fd,buffer,size);
            Scheduler::instance().terminate();
            break;
        case Scheduler::EV_HUP:
        case Scheduler::EV_ERR:
        case Scheduler::EV_NONE:
        case Scheduler::EV_ALL:
            ;
        }
        Scheduler::instance().terminate();
    }

    void timeout() 
    {
	Scheduler::instance().terminate();
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

    void handleCallback(HandleWrapper const & handle, Scheduler::EventId event)
    {
        if (handle.tag_ != "TheTag")
            return;
        callback(handle.fd_,event);
    }

    bool is_close(MicroTime a, MicroTime b)
    {
	return (a<b ? b-a : a-b) < 10100; // a little bit over 10ms
    }
	    
}

BOOST_AUTO_UNIT_TEST(scheduler)
{
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

    ///////////////////////////////////////////////////////////////////////////

    BOOST_CHECK_NO_THROW( Scheduler::instance() );

    BOOST_CHECK_NO_THROW( Scheduler::instance().add(sock,&callback,Scheduler::EV_READ) );
    event = Scheduler::EV_NONE;
    BOOST_CHECK_NO_THROW( Scheduler::instance().process() );
    BOOST_CHECK_EQUAL( event, Scheduler::EV_READ );
    BOOST_REQUIRE_EQUAL( size, 4 );
    buffer[size]=0;
    BOOST_CHECK_EQUAL( buffer, "READ" );

    BOOST_CHECK_NO_THROW( Scheduler::instance().timeout(100,&timeout) );
    BOOST_CHECK_NO_THROW( Scheduler::instance().timeout(200,&timeout) );
    MicroTime t (now());
    BOOST_CHECK_NO_THROW( Scheduler::instance().process() );
    BOOST_CHECK_PREDICATE( is_close, (now()) (t+100*1000) );
    BOOST_CHECK_NO_THROW( Scheduler::instance().process() );
    BOOST_CHECK_PREDICATE( is_close, (now()) (t+200*1000) );
    
    HandleWrapper handle(sock,"TheTag");
    BOOST_CHECK_NO_THROW( Scheduler::instance().add(handle,&handleCallback,Scheduler::EV_WRITE) );
    strcpy(buffer,"WRITE");
    size=5;
    event = Scheduler::EV_NONE;
    BOOST_CHECK_NO_THROW( Scheduler::instance().process() );
    BOOST_CHECK_EQUAL( event, Scheduler::EV_WRITE );

    BOOST_CHECK_NO_THROW( Scheduler::instance().remove(handle,Scheduler::EV_WRITE) );
    event = Scheduler::EV_NONE;
    BOOST_CHECK_NO_THROW( Scheduler::instance().process() );
    BOOST_CHECK_EQUAL( event, Scheduler::EV_READ );
    BOOST_REQUIRE_EQUAL( size, 2 );
    buffer[size]=0;
    BOOST_CHECK_EQUAL( buffer, "OK" );

    ///////////////////////////////////////////////////////////////////////////

    close(sock);

    BOOST_CHECK (stop_server(pid));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
