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
    \brief Target unit tests */

//#include "Target.test.hh"
//#include "Target.test.ih"

// Custom includes
#include <sstream>
#include "main.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct RouteCheck
    {
        typedef std::string result_type;
        std::string operator()(senf::log::Target::RoutingEntry const & entry) const
            {
                static char const * levels[] = {
                        "NONE", "VERBOSE", "NOTICE", "MESSAGE", "IMPORTANT", "CRITICAL", "FATAL", "DISABLED" };
                static char const * actions[] = { "ACCEPT", "REJECT" };
                std::stringstream s;
                s << entry.stream() << "-" << entry.area() << "-" << levels[entry.level()] << "-"
                  << actions[entry.action()];
                return s.str();
            }
    };

}

SENF_AUTO_UNIT_TEST(target)
{
    senf::log::StringTarget target;

    BOOST_CHECK_THROW( target.route("senf::log::test::myStream", "invalid_area"),
            senf::log::Target::InvalidAreaException );
    BOOST_CHECK_THROW( target.route("invalid_stream", ""),
            senf::log::Target::InvalidStreamException );
    BOOST_CHECK_THROW( target.unroute("senf::log::test::myStream", "invalid_area"),
            senf::log::Target::InvalidAreaException );
    BOOST_CHECK_THROW( target.unroute("invalid_stream", ""),
            senf::log::Target::InvalidStreamException );

    target.route<senf::log::Debug>();
    target.route<senf::log::test::myStream, senf::log::DefaultArea>(senf::log::Target::REJECT);
    target.route<senf::log::test::myStream, senf::log::VERBOSE>(senf::log::Target::ACCEPT, 0);
    target.route<senf::log::test::myStream, senf::log::test::Foo, senf::log::VERBOSE>(
        senf::log::Target::ACCEPT, 2);
    target.route("senf::log::test::myStream", "", senf::log::IMPORTANT::value,
                 senf::log::Target::REJECT, 4);
    target.route("senf::log::Debug", "senf::log::test::Foo", senf::log::VERBOSE::value,
                 senf::log::Target::REJECT, -5);
    target.route("senf::log::Debug", "", senf::log::MESSAGE::value,
                 senf::log::Target::ACCEPT, -7);

    typedef boost::transform_iterator<RouteCheck, senf::log::Target::iterator> iterator;
    iterator i (boost::make_transform_iterator(target.begin(), RouteCheck()));
    iterator const i_end (boost::make_transform_iterator(target.end(), RouteCheck()));

    char const * data[] = {
        "senf::log::Debug--MESSAGE-ACCEPT",
        "senf::log::test::myStream--VERBOSE-ACCEPT",
        "senf::log::Debug-senf::log::test::Foo-VERBOSE-REJECT",
        "senf::log::Debug--NONE-ACCEPT",
        "senf::log::test::myStream-senf::log::test::Foo-VERBOSE-ACCEPT",
        "senf::log::test::myStream-senf::log::DefaultArea-NONE-REJECT",
        "senf::log::test::myStream--IMPORTANT-REJECT",
    };

    BOOST_CHECK_EQUAL_COLLECTIONS( i, i_end, data, data + sizeof(data)/sizeof(data[0]) );
    BOOST_CHECK( *target.begin() == target[0] );

    target.unroute<senf::log::Debug>();
    target.unroute<senf::log::test::myStream, senf::log::VERBOSE>();
    target.unroute<senf::log::test::myStream, senf::log::DefaultArea>(senf::log::Target::REJECT);
    target.unroute("senf::log::test::myStream", "", senf::log::IMPORTANT::value,
                   senf::log::Target::REJECT);
    target.unroute(1);
    target.flush();

    BOOST_CHECK( target.begin() == target.end() );
    BOOST_CHECK( target.empty() );
    BOOST_CHECK_EQUAL( target.size(), 0u );
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
