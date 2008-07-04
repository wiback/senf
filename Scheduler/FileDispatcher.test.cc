// $Id$
//
// Copyright (C) 2008 
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
    \brief FileDispatcher.test unit tests */

//#include "FileDispatcher.test.hh"
//#include "FileDispatcher.test.ih"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "FileDispatcher.hh"
#include "ClockService.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    bool is_close(senf::ClockService::clock_type a, senf::ClockService::clock_type b)
    {
        return (a<b ? b-a : a-b) < senf::ClockService::milliseconds(100);
    }

    bool called (false);
    void handler(int events)
    {
        called=true;
    }
}

BOOST_AUTO_UNIT_TEST(fileDispatcher)
{
    senf::scheduler::FdManager manager;
    senf::scheduler::FIFORunner runner;
    senf::scheduler::FileDispatcher dispatcher (manager, runner);
    dispatcher.timeout(500);

    // It's not necessary for the cb to be a real file .. it can be anything. The only thing is,
    // it's always called without delay.
    int fd (open("/dev/null", O_RDWR));
    
    senf::ClockService::clock_type t (senf::ClockService::now());
    SENF_CHECK_NO_THROW( dispatcher.add(fd, &handler, senf::scheduler::FileDispatcher::EV_READ) );
    SENF_CHECK_NO_THROW( manager.processOnce() );
    SENF_CHECK_NO_THROW( dispatcher.prepareRun() );
    SENF_CHECK_NO_THROW( runner.run() );
    
    BOOST_CHECK( called );
    BOOST_CHECK_PREDICATE( is_close, (t)(senf::ClockService::now()) );

    SENF_CHECK_NO_THROW( dispatcher.remove(fd) );
    close(fd);

    called = false;
    t = senf::ClockService::now();
    SENF_CHECK_NO_THROW( manager.processOnce() );
    SENF_CHECK_NO_THROW( dispatcher.prepareRun() );
    SENF_CHECK_NO_THROW( runner.run() );

    BOOST_CHECK( ! called );
    BOOST_CHECK_PREDICATE( 
        is_close, (t+senf::ClockService::milliseconds(500))(senf::ClockService::now()) );
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
