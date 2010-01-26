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
    \brief Log unit tests */

//#include "Log.test.hh"
//#include "Log.test.ih"

// Custom includes
#include "main.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(logger)
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
    std::cerr << target.str();
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG((senf::log::test::LogCritical) ("Another log message: " << 10));
    std::cerr << target.str();
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG_BLOCK((senf::log::Debug) (senf::log::IMPORTANT) ({
        log << "Last message";
        log << " continued here";
    }));
    std::cerr << target.str();
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    senf::log::test::Foo::log();
    std::cerr << target.str();
    BOOST_CHECK( ! target.str().empty() );
    target.clear();

    SENF_LOG((senf::log::test::Foo)("Foo area"));
    BOOST_CHECK( target.str().empty() );
    target.clear();
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
