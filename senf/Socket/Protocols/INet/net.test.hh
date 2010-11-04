// $Id$
//
// Copyright (C) 2009
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
    \brief net.test public header */

#ifndef HH_SENF_senf_Socket_Protocols_INet_net_test_
#define HH_SENF_senf_Socket_Protocols_INet_net_test_ 1

// Custom includes
#include <boost/format.hpp>
#include <boost/test/test_tools.hpp>

//#include "net.test.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

    int base_pid = 0;
    int server_pid = 0;

    void start(void (*fn)())
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

    unsigned port(unsigned i)
    {
        if (! base_pid)
            base_pid = ::getpid();
        return 23456u + (((base_pid^(base_pid>>8)^(base_pid>>16)^(base_pid>>24))&0xff)<<2) + i;
    }

    std::string localhost4str(unsigned i)
    {
        return (boost::format("localhost:%d") % port(i)).str();
    }

    std::string localhost6str(unsigned i)
    {
        return (boost::format("[::1]:%d") % port(i)).str();
    }

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "net.test.cci"
//#include "net.test.ct"
//#include "net.test.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
