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
    \brief Target unit tests */

//#include "Target.test.hh"
//#include "Target.test.ih"

// Custom includes
#include <sstream>
#include "main.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
