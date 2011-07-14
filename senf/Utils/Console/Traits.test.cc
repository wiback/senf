// $Id$
//
// Copyright (C) 2008
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
    \brief Traits unit tests */

//#include "Traits.test.hh"
//#include "Traits.test.ih"

// Custom includes
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    char charTest(char value) { return value; }
    signed char scharTest(signed char value) { return value; }
    unsigned char ucharTest(unsigned char value) { return value; }

    bool boolTest(bool value) { return value; }

    enum TestEnum { Foo=1, Bar=2, FooBar=4 };
    SENF_CONSOLE_REGISTER_ENUM( TestEnum, (Foo)(Bar)(FooBar) );

    TestEnum test (TestEnum value) { return value; }

    struct TestClass {
        enum MemberEnum { MemberFoo, MemberBar };
        static MemberEnum test (MemberEnum value) { return value; }
    };
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( TestClass, MemberEnum, (MemberFoo)(MemberBar) );

    enum TestEnumKey { ONE, TWO, THREE };
    SENF_CONSOLE_REGISTER_ENUM( TestEnumKey, (key("1s",ONE))(key("2",TWO))(key("three",THREE)) );

    TestEnumKey testKey (TestEnumKey value) { return value; }
}

SENF_AUTO_UNIT_TEST(charTraits)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    std::stringstream ss;

    dir.add("test",fty::Command(&charTest));
    dir.add("stest",fty::Command(&scharTest));
    dir.add("utest",fty::Command(&ucharTest));

    SENF_CHECK_NO_THROW(
        parser.parse("test/test 10; test/test 20",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "10\n" "20\n" );
    ss.str("");

    SENF_CHECK_NO_THROW(
        parser.parse("test/stest 10; test/stest -20",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "10\n" "-20\n" );
    ss.str("");

    SENF_CHECK_NO_THROW(
        parser.parse("test/utest 10; test/utest 20",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "10\n" "20\n" );
    ss.str("");
}

SENF_AUTO_UNIT_TEST(boolTraits)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    dir.add("test", fty::Command(&boolTest));

    std::stringstream ss;
    SENF_CHECK_NO_THROW(
        parser.parse("test/test true; test/test false",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "true\n" "false\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test enabled; test/test disabled",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "true\n" "false\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test yes; test/test no",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "true\n" "false\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test Y; test/test enA",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "true\n" "true\n" );

    dir.add("test2", fty::Command(&boolTest).formatter( senf::console::formatEnabledDisabled ));
    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test2 0; test/test2 -1",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "disabled\n" "enabled\n" );
}

SENF_AUTO_UNIT_TEST(stringTraits)
{
    BOOST_CHECK_EQUAL(
        senf::console::ArgumentTraits<std::string>::str("Test\nOf\nA \"String\"\x01\x7f\xa0\xff"),
        "\"Test\\x0aOf\\x0aA \\\"String\\\"\\x01\\x7f\\xa0\\xff\"" );
}

SENF_AUTO_UNIT_TEST(enumSupport)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    dir.add("test",fty::Command(&test));
    dir.add("testKey",fty::Command(&testKey));

    std::stringstream ss;
    SENF_CHECK_NO_THROW(
        parser.parse("test/test Foo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Foo\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test Bar",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Bar\n" );

    BOOST_CHECK_THROW(
        parser.parse("test/test (Foo Bar)",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
        senf::console::SyntaxErrorException );

    BOOST_CHECK_THROW(
        parser.parse("test/test Baz",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
        senf::console::SyntaxErrorException );

    dir.add("member", fty::Command(&TestClass::test));

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/member MemberFoo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "MemberFoo\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/member MemberBar",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "MemberBar\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test foob",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "FooBar\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test b",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Bar\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test foo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Foo\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/testKey 1S",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "1s\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/testKey 2",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "2\n" );

    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/testKey ThrEE",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "three\n" );
}

SENF_AUTO_UNIT_TEST(singleToken)
{
    BOOST_CHECK( senf::console::ArgumentTraits<std::string>::singleToken );
    BOOST_CHECK( senf::console::ArgumentTraits<int>::singleToken );
    BOOST_CHECK( ! senf::console::ArgumentTraits<
                       senf::console::FlagCollection<TestEnum> >::singleToken );
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
