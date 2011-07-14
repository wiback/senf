// $Id$
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Daemon unit tests */

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
#include <senf/Utils/Exception.hh>
#include <senf/Utils/Backtrace.hh>
#include <senf/Scheduler/Scheduler.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
            std::cerr << "(stderr)" << std::endl;
        }

        void run() {
            std::cout << "Running run()" << std::endl;
            delay(2000);
        }
    };

    int myMain(int argc, char const ** argv)
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

    int run(int argc, char const ** argv)
    {
        pid  = ::fork();
        if (pid < 0) throw senf::SystemException("::fork()");
        if (pid == 0) {
            signal(SIGABRT, &backtrace);
            signal(SIGCHLD, SIG_IGN);
            try {
                ::_exit(myMain(argc, argv));
            } catch (std::exception & ex) {
                std::cerr << "Unexpected exception: " << ex.what() << std::endl;
            } catch (...) {
                std::cerr << "Unexpected exception" << std::endl;
            }
            ::_exit(125);
        }
        signal(SIGCHLD, SIG_DFL);
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

SENF_AUTO_UNIT_TEST(testDaemon)
{
    char const * args[] = { "run",
                            "--console-log=testDaemon.log",
                            "--pid-file=testDaemon.pid" };

    SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( run(sizeof(args)/sizeof(*args), args), 0 ) );

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

    delay(3000);
    BOOST_CHECK( ! boost::filesystem::exists("testDaemon.pid") );
    BOOST_CHECK( boost::filesystem::exists("testDaemon.log") );
    BOOST_CHECK( boost::filesystem::exists("testDaemon.log.1") );

    std::ifstream log ("testDaemon.log.1");
    std::stringstream data;
    data << log.rdbuf();
    BOOST_CHECK_EQUAL( data.str(), "Running init()\n(stderr)\nRunning run()\n" );
    SENF_CHECK_NO_THROW( boost::filesystem::remove("testDaemon.log") );
    SENF_CHECK_NO_THROW( boost::filesystem::remove("testDaemon.log.1") );
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
