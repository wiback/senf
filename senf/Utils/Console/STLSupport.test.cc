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
    \brief STLSupport unit tests */

//#include "STLSupport.test.hh"
//#include "STLSupport.test.ih"

// Custom includes
#include <boost/assign/list_of.hpp>
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    template <class Container>
    struct Summer
    {
        static int test(Container const & data)
        {
            int sum (0);
            for (typename Container::const_iterator i (data.begin()), i_end (data.end());
                 i != i_end; ++i)
                sum += *i;
            return sum;
        }
    };

    std::pair<std::string, int> mapTest(std::map<std::string,int> const & data)
    {
        std::string keys;
        int sum (0);
        for (std::map<std::string,int>::const_iterator i (data.begin()), i_end (data.end());
             i != i_end; ++i) {
            keys += i->first;
            sum += i->second;
        }
        return std::make_pair(keys,sum);
    }

}

SENF_AUTO_UNIT_TEST(vectorSupport)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    std::vector<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", fty::Command(&Summer<std::vector<int> >::test)
        .arg("data", "test data", senf::console::kw::default_value = defv)
        );
    std::stringstream ss;

    SENF_CHECK_NO_THROW(
        parser.parse("test/test; test/test (); test/test 5; test/test (13); test/test (4 5 8)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "9\n" "0\n" "5\n" "13\n" "17\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("help test/test",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL(
        ss.str(),
        "Usage:\n"
        "    test [data:vector<int>]\n"
        "\n"
        "With:\n"
        "    data      test data\n"
        "        default: (7 2)\n" );
}

SENF_AUTO_UNIT_TEST(listSupport)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    std::list<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", fty::Command(&Summer<std::list<int> >::test)
        .arg("data", "test data", senf::console::kw::default_value = defv)
        );
    std::stringstream ss;

    SENF_CHECK_NO_THROW(
        parser.parse("test/test; test/test (); test/test 5; test/test (13); test/test (4 5 8)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "9\n" "0\n" "5\n" "13\n" "17\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("help test/test",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL(
        ss.str(),
        "Usage:\n"
        "    test [data:list<int>]\n"
        "\n"
        "With:\n"
        "    data      test data\n"
        "        default: (7 2)\n" );
}

SENF_AUTO_UNIT_TEST(setSupport)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    std::set<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", fty::Command(&Summer<std::set<int> >::test)
        .arg("data", "test data", senf::console::kw::default_value = defv)
        );
    std::stringstream ss;

    SENF_CHECK_NO_THROW(
        parser.parse("test/test; test/test (); test/test 5; test/test (13); test/test (4 5 8)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "9\n" "0\n" "5\n" "13\n" "17\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("help test/test",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL(
        ss.str(),
        "Usage:\n"
        "    test [data:set<int>]\n"
        "\n"
        "With:\n"
        "    data      test data\n"
        "        default: (2 7)\n" );
}

SENF_AUTO_UNIT_TEST(mapSupport)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    std::map<std::string, int> defv (
        boost::assign::map_list_of("foo bar",7)("bar",2).to_container(defv));
    dir.add("test", fty::Command(&mapTest)
        .arg("data", "test data", senf::console::kw::default_value = defv)
        );
    std::stringstream ss;

    SENF_CHECK_NO_THROW(
        parser.parse("test/test; test/test (); "
                     "test/test (vier=4 fuenf = 5 acht=8 )",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "(\"barfoo bar\" 9)\n" "(\"\" 0)\n" "(achtfuenfvier 17)\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("help test/test",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL(
        ss.str(),
        "Usage:\n"
        "    test [data:map<string,int>]\n"
        "\n"
        "With:\n"
        "    data      test data\n"
        "        default: (bar=2 \"foo bar\"=7)\n" );
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
