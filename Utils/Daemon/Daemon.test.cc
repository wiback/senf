// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

/** \file
    \brief Daemon.test unit tests */

//#include "Daemon.test.hh"
//#include "Daemon.test.ih"

// Custom includes
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include "Daemon.hh"
#include "../Utils/Exception.hh"
#include "../Utils/Backtrace.hh"
#include "../Scheduler/Scheduler.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    void delay(unsigned long milliseconds)
    {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        while (nanosleep(&ts,&ts) < 0 && errno == EINTR) ;
    }

    class MyDaemon : public senf::Daemon
    {
        void configure() { 
            std::cout << "Running configure()" << std::endl; 
            pidFile("invalid.pid");
            consoleLog("invalid.log");
            senf::Daemon::configure();
        }

        void init() { 
            std::cout << "Running init()" << std::endl; 
        }

        void run() {
            std::cout << "Running run()" << std::endl; 
            delay(1500);
        }
    };

    int myMain(int argc, char ** argv)
    {
        MyDaemon instance;
        return instance.start(argc, argv);
    }

    int pid;

    void backtrace(int)
    {
        senf::backtrace(std::cerr, 100);
        ::signal(SIGABRT, SIG_DFL);
        ::kill(::getpid(), SIGABRT);
    };

    int run(int argc, char ** argv)
    {
        pid  = ::fork();
        if (pid < 0) throw senf::SystemException("::fork()");
        if (pid == 0) {
            signal(SIGABRT, &backtrace);
            try {
                ::_exit(myMain(argc, argv));
            } catch (std::exception & ex) {
                std::cerr << "Unexpected exception: " << ex.what() << std::endl;
            } catch (...) {
                std::cerr << "Unexpected exception" << std::endl;
            }
            ::_exit(125);
        }
        int status;
        if (::waitpid(pid, &status, 0) < 0) 
            throw senf::SystemException("::waitpid()");
        if (WIFSIGNALED(status))
            std::cerr << "Terminated with signal " 
                      << senf::signalName(WTERMSIG(status)) << "(" << WTERMSIG(status) << ")\n";
        else if (WIFEXITED(status))
            std::cerr << "Exited normally with exit status " << WEXITSTATUS(status) << "\n";
        return status;
    }

}

BOOST_AUTO_UNIT_TEST(testDaemon)
{
    char * args[] = { "run", 
                      "--console-log=testDaemon.log", 
                      "--pid-file=testDaemon.pid" };
    BOOST_CHECK_EQUAL( run(sizeof(args)/sizeof(*args),args), 0 );

    BOOST_CHECK( ! boost::filesystem::exists("invalid.log") );
    BOOST_CHECK( ! boost::filesystem::exists("invalid.pid") );
    BOOST_CHECK( boost::filesystem::exists("testDaemon.pid") );
    BOOST_REQUIRE( boost::filesystem::exists("testDaemon.log") );
    
    boost::filesystem::rename("testDaemon.log", "testDaemon.log.1");
    {
        std::ifstream pidFile ("testDaemon.pid");
        int pid (0);
        BOOST_CHECK( pidFile >> pid );
        BOOST_CHECK( pid != 0 );
        if (pid != 0)
            ::kill(pid, SIGHUP);
    }

    delay(1000);
    BOOST_CHECK( ! boost::filesystem::exists("testDaemon.pid") );
    BOOST_CHECK( boost::filesystem::exists("testDaemon.log") );
    BOOST_CHECK( boost::filesystem::exists("testDaemon.log.1") );
    
    std::ifstream log ("testDaemon.log.1");
    std::stringstream data;
    data << log.rdbuf();
    BOOST_CHECK_EQUAL( data.str(), "Running init()\nRunning run()\n" );
    BOOST_CHECK_NO_THROW( boost::filesystem::remove("testDaemon.log") );
    BOOST_CHECK_NO_THROW( boost::filesystem::remove("testDaemon.log.1") );
}

///////////////////////////////cc.e////////////////////////////////////////
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
