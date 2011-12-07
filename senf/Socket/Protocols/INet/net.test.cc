// $Id$
//
// Copyright (C) 2009
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
    \brief net.test public header */

#include "net.test.hh"

// Custom includes
#include <iostream>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <boost/format.hpp>
#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
    int base_pid = 0;
    int server_pid = 0;
}

void senf::test::error(char const * fn, char const * proc)
{
    std::cerr << "\n" << proc << ((*proc)?": ":"") << fn << ": " << strerror(errno) << std::endl;
}

void senf::test::fail(char const * proc, char const * fn)
{
    error(fn,proc);
    _exit(1);
}

void senf::test::start(void (*fn)())
{
    if (! base_pid)
        base_pid = ::getpid();
    server_pid = ::fork();
    if (server_pid < 0) BOOST_FAIL("fork()");
    if (server_pid == 0) {
        signal(SIGCHLD, SIG_IGN);
        (*fn)();
        _exit(0);
    }
    signal(SIGCHLD, SIG_DFL);
    sleep(1);
}

void senf::test::wait()
{
    int status;
    if (waitpid(server_pid,&status,0)<0)
        BOOST_FAIL("waitpid()");
    BOOST_CHECK_EQUAL( status , 0 );
}

void senf::test::stop()
{
    if (server_pid) {
        kill(server_pid,9);
        wait();
        server_pid = 0;
    }
}

unsigned senf::test::port(unsigned i)
{
    if (! base_pid)
        base_pid = ::getpid();
    return 23456u + (((base_pid^(base_pid>>8)^(base_pid>>16)^(base_pid>>24))&0xff)<<2) + i;
}

std::string senf::test::localhost4str(unsigned i)
{
    return (boost::format("localhost:%d") % port(i)).str();
}

std::string senf::test::localhost6str(unsigned i)
{
    return (boost::format("[::1]:%d") % port(i)).str();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "net.test.cci"
//#include "net.test.ct"
//#include "net.test.cti"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
