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
    \brief Utility.test unit tests */

// Custom includes
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    char charTest(char value) { return value; }

    enum TestEnum { Foo=1, Bar=2, FooBar=4 };
    SENF_CONSOLE_REGISTER_ENUM( TestEnum, (Foo)(Bar)(FooBar) );

    senf::console::FlagCollection<TestEnum> collectionTest(
            senf::console::FlagCollection<TestEnum> flags) { return flags; }

#ifdef SENF_CXX11_ENABLED
    enum struct TestStructEnum : std::uint8_t { Ham=1, Eggs=2, Spam=4 };
    SENF_CONSOLE_REGISTER_STRONG_ENUM( TestStructEnum, (Ham)(Eggs)(Spam) );

    senf::console::FlagCollection<TestStructEnum> collectionTest2_1(
            senf::console::FlagCollection<TestStructEnum> flags) { return flags; }
    std::uint8_t collectionTest2_2(
            senf::console::FlagCollection<TestStructEnum> flags) { return flags.value; }
#endif

}

SENF_AUTO_TEST_CASE(charAsString)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    std::stringstream ss;

    dir.add("test",
            fty::Command<senf::console::CharAsString<char> (senf::console::CharAsString<char>)>(
                &charTest));

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test \"\\x01\"",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "\x01\n" );
}

SENF_AUTO_TEST_CASE(flagCollection)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    std::stringstream ss;

    dir.add("test",fty::Command(&collectionTest));

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test foo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Foo\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test (foo bar)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "(Foo Bar)\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test ()",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "()\n" );

    BOOST_CHECK_EQUAL( senf::str(senf_console_format_enum(Foo)), "Foo");

#ifdef SENF_CXX11_ENABLED
    dir.add("test2_1",fty::Command(&collectionTest2_1));
    dir.add("test2_2",fty::Command(&collectionTest2_2));

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test2_1 (Ham Eggs)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "(Ham Eggs)\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test2_2 (Ham Eggs)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "3\n" );

    typedef std::is_same<senf::console::FlagCollection<TestStructEnum>::underlying_type, std::uint8_t> is_same;
    BOOST_CHECK( is_same::value );
#endif
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
