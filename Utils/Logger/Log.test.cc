// $Id: Log.test.cc 369 2007-08-01 07:51:36Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Log.test unit tests */

//#include "Log.test.hh"
//#include "Log.test.ih"

// Custom includes
#include <sstream>

#define _senf_LOG_STREAM logstream
namespace {
    std::stringstream logstream;
}

#define SENF_LOG_CONF ((senf::log::Debug)(_)(VERBOSE))

#include "Log.hh"
#include "Defaults.hh"
#include "Parameters.hh"
#include "Levels.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    template <class T>
    struct Foo
    {
        typedef int value;
    };

    SENF_LOG_DEF_ALIAS( LogFoo, (senf::log::Debug) (senf::log::CRITICAL) );
    SENF_LOG_DEF_STREAM( myStream, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE );
    SENF_LOG_DEF_AREA( myArea );

}

BOOST_AUTO_UNIT_TEST(logger)
{
    SENF_LOG_DEFAULT_STREAM(senf::log::Debug);
    SENF_LOG_DEFAULT_AREA(senf::log::DefaultArea);
    SENF_LOG_DEFAULT_LEVEL(senf::log::NOTICE);

    SENF_LOG(("Log message"));

    SENF_LOG((LogFoo) ("Another log message: " << 10));

    SENF_LOG_BLOCK((senf::log::Debug) (senf::log::IMPORTANT) ({
        log << "Last message";
        log << " continued here";
    }));

    BOOST_CHECK_EQUAL( logstream.str(), 
                       "Log message\nAnother log message: 10\nLast message continued here\n" );
}

///////////////////////////////cc.e////////////////////////////////////////
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
