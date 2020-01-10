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
    \brief Traits unit tests */

// Custom includes
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

namespace myTest {

    struct TestStruct
    {
        int a;
        std::string b;
        int c;
    };

    struct TestStruct2 : public TestStruct {};

    template <typename T>
    T testParsing(T t)
    {
        t.a += t.a;
        t.b += t.b;
        t.c += t.c;
        return t;
    }
}

SENF_CONSOLE_PARSE_AND_FORMAT_AS_TUPLE( myTest::TestStruct, (a)(b)(c));
SENF_CONSOLE_PARSE_AND_FORMAT_AS_MAP( myTest::TestStruct2, (("a", a))(("b", b))(("c", c)));

SENF_AUTO_TEST_CASE(parse_and_format_as_tuple_macros)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    dir.add("tuple",fty::Command(&myTest::testParsing<myTest::TestStruct>));
    dir.add("map",fty::Command(&myTest::testParsing<myTest::TestStruct2>));

    {
        std::stringstream ss;
        SENF_CHECK_NO_THROW(
                parser.parse("test/tuple (42 \"a b c\" 23)",
                        boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "(84 \"a b ca b c\" 46)\n" );
    } {
        std::stringstream ss;
        SENF_CHECK_NO_THROW(
                parser.parse("test/map (a=42 b=\"a b c\" c=23)",
                        boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "(a=84 b=\"a b ca b c\" c=46)\n" );
    } {
        std::stringstream ss;
        BOOST_CHECK_THROW(
                parser.parse("test/map (x=42 b=\"a b c\" c=23)",
                        boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
                senf::console::SyntaxErrorException );
        BOOST_CHECK_THROW(
                parser.parse("test/map (a=42 b=\"a b c\")",
                        boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
                senf::console::SyntaxErrorException );
    }
}

SENF_AUTO_TEST_CASE(charTraits)
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

SENF_AUTO_TEST_CASE(boolTraits)
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

SENF_AUTO_TEST_CASE(stringTraits)
{
    BOOST_CHECK_EQUAL(
        senf::console::ArgumentTraits<std::string>::str("Test\nOf\nA \"String\"\x01\x7f\xa0\xff"),
        "\"Test\\x0aOf\\x0aA \\\"String\\\"\\x01\\x7f\\xa0\\xff\"" );
}

SENF_AUTO_TEST_CASE(enumSupport)
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

    BOOST_CHECK_EQUAL( senf::str(senf_console_format_enum(Foo)), "Foo");
    BOOST_CHECK_EQUAL( senf::str(senf_console_format_enum(TestClass::MemberBar)), "MemberBar");
    BOOST_CHECK_EQUAL( senf::str(senf_console_format_enum(TWO)), "2");
}

SENF_AUTO_TEST_CASE(singleToken)
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
