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
    \brief Log unit tests */

#include "main.test.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(logger)
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
