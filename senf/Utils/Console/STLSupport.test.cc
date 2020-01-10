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
    \brief STLSupport unit tests */

// Custom includes
#include <boost/assign/list_of.hpp>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

namespace console = senf::console;


SENF_AUTO_TEST_CASE(vectorSupport)
{
    console::Executor executor;
    console::CommandParser parser;
    console::ScopedDirectory<> dir;
    console::root().add("test", dir);

    std::vector<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", console::factory::Command(&Summer<std::vector<int> >::test)
        .arg("data", "test data", console::kw::default_value = defv) );
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

SENF_AUTO_TEST_CASE(listSupport)
{
    console::Executor executor;
    console::CommandParser parser;
    console::ScopedDirectory<> dir;
    console::root().add("test", dir);

    std::list<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", console::factory::Command(&Summer<std::list<int> >::test)
        .arg("data", "test data", console::kw::default_value = defv) );
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

SENF_AUTO_TEST_CASE(setSupport)
{
    console::Executor executor;
    console::CommandParser parser;
    console::ScopedDirectory<> dir;
    console::root().add("test", dir);

    std::set<int> defv (boost::assign::list_of(7)(2).to_container(defv));
    dir.add("test", console::factory::Command(&Summer<std::set<int> >::test)
        .arg("data", "test data", console::kw::default_value = defv) );
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

SENF_AUTO_TEST_CASE(mapSupport)
{
    console::Executor executor;
    console::CommandParser parser;
    console::ScopedDirectory<> dir;
    console::root().add("test", dir);

    std::map<std::string, int> defv (
        boost::assign::map_list_of("foo bar",7)("bar",2).to_container(defv));
    dir.add("test", console::factory::Command(&mapTest)
        .arg("data", "test data", console::kw::default_value = defv) );
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

#ifdef SENF_CXX11_ENABLED

namespace {

    std::tuple<int, std::pair<int,int>, int> tupleTest(std::tuple<int, std::string, int> data)
    {
        return std::make_tuple(
                std::get<0>(data),
                std::make_pair(std::stoi(std::get<1>(data)), std::stoi(std::get<1>(data))),
                std::get<0>(data) + std::get<2>(data) );
    }
}

SENF_AUTO_TEST_CASE(tupleSupport)
{
    console::Executor executor;
    console::CommandParser parser;
    console::ScopedDirectory<> dir;
    console::root().add("test", dir);
    dir.add("test", console::factory::Command(&tupleTest)
        .arg("data", "test data", console::kw::default_value = std::make_tuple(0, "Test", 0)) );

    std::stringstream ss;
    SENF_CHECK_NO_THROW(
        parser.parse("test/test (42 \"23\" 18)",
                boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "(42 (23 23) 60)\n");

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("help test/test",
                boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL(
        ss.str(),
        "Usage:\n"
        "    test [data:tuple<int,string,int>]\n"
        "\n"
        "With:\n"
        "    data      test data\n"
        "        default: (0 Test 0)\n" );
}

#endif

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
