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

// We need to put all tests into this single file to not violate the ODR

#define SENF_LOG_CONF (( (senf)(log)(Debug), (_), NOTICE ))

// Custom includes
#include "Logger.hh"
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace not_anonymous {
    
    struct Foo
    {
        SENF_LOG_CLASS_AREA();

        static void log() {
            SENF_LOG(("Foo::log"));
        }
    };

    SENF_LOG_DEF_ALIAS( LogCritical, (senf::log::Debug) (senf::log::CRITICAL) );
    SENF_LOG_DEF_STREAM( myStream, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE );
    SENF_LOG_DEF_AREA( myArea );

}
using namespace not_anonymous;

BOOST_AUTO_UNIT_TEST(logger)
{
    senf::log::StringTarget target;

    target.route<senf::log::Debug>();

    // We cannot easily check the exact log string since that includes the current date/time
    
    SENF_LOG_DEFAULT_STREAM(senf::log::Debug);
    SENF_LOG_DEFAULT_AREA(senf::log::DefaultArea);
    SENF_LOG_DEFAULT_LEVEL(senf::log::VERBOSE);

    SENF_LOG(("Log message"));
    BOOST_CHECK( target.str().empty() );
    target.clear();

    SENF_LOG((senf::log::VERBOSE)("Log message 2"));
    BOOST_CHECK( target.str().empty() );
    target.clear();
    
    SENF_LOG((senf::log::IMPORTANT)("Important message"));
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG((LogCritical) ("Another log message: " << 10));
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG_BLOCK((senf::log::Debug) (senf::log::IMPORTANT) ({
        log << "Last message";
        log << " continued here";
    }));
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    Foo::log();
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG((Foo)("Foo area"));
    BOOST_CHECK( target.str().empty() );
    target.clear();
}

BOOST_AUTO_UNIT_TEST(streamRegistry)
{
    char const * streams[] = { "not_anonymous::myStream", "senf::log::Debug" };

    BOOST_CHECK_EQUAL_COLLECTIONS( senf::log::StreamRegistry::instance().begin(),
                                   senf::log::StreamRegistry::instance().end(),
                                   streams, streams+sizeof(streams)/sizeof(streams[0]) );
    BOOST_CHECK_EQUAL( senf::log::detail::StreamBase::nStreams, 2u );
}

BOOST_AUTO_UNIT_TEST(areaRegistry)
{
    char const * areas[] = { "", "not_anonymous::Foo", "not_anonymous::myArea" };

    BOOST_CHECK_EQUAL_COLLECTIONS( senf::log::AreaRegistry::instance().begin(),
                                   senf::log::AreaRegistry::instance().end(),
                                   areas, areas+sizeof(areas)/sizeof(areas[0]) );

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
