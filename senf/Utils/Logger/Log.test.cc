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
    \brief Log unit tests */

//#include "Log.test.hh"
//#include "Log.test.ih"

// Custom includes
#include "main.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
