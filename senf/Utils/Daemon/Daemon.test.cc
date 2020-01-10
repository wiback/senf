//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Daemon unit tests */

#include "Daemon.hh"

// Custom includes
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/Backtrace.hh>
#include <senf/Scheduler/Scheduler.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(testDaemon)
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
