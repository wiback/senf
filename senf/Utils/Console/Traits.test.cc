// $Id$
//
// Copyright (C) 2008 
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
    \brief Traits unit tests */

//#include "Traits.test.hh"
//#include "Traits.test.ih"

// Custom includes
#include "Traits.hh"
#include "ParsedCommand.hh"
#include "Executor.hh"
#include "Parse.hh"
#include "ScopedDirectory.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    enum TestEnum { Foo=1, Bar=2, FooBar=4 };
    SENF_CONSOLE_REGISTER_ENUM( TestEnum, (Foo)(Bar)(FooBar) );

    TestEnum test (TestEnum value) { return value; }

    struct TestClass {
        enum MemberEnum { MemberFoo, MemberBar };
        static MemberEnum test (MemberEnum value) { return value; }
    };
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( TestClass, MemberEnum, (MemberFoo)(MemberBar) );

    bool boolTest(bool value) { return value; }

    senf::console::FlagCollection<TestEnum> collectionTest(
        senf::console::FlagCollection<TestEnum> flags) { return flags; }
}

BOOST_AUTO_UNIT_TEST(boolTraits)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    dir.add("test", &boolTest);

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

    dir.add("test2", &boolTest).formatter( senf::console::formatEnabledDisabled );
    ss.str("");
    SENF_CHECK_NO_THROW(
        parser.parse("test/test2 0; test/test2 -1",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "disabled\n" "enabled\n" );
}

BOOST_AUTO_UNIT_TEST(enumSupport)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    dir.add("test",&test);
    
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

    dir.add("member", &TestClass::test);

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
}

BOOST_AUTO_UNIT_TEST(flagCollection)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    std::stringstream ss;

    dir.add("test",&collectionTest);
    
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
}

BOOST_AUTO_UNIT_TEST(singleToken)
{
    BOOST_CHECK( senf::console::ArgumentTraits<std::string>::singleToken );
    BOOST_CHECK( senf::console::ArgumentTraits<int>::singleToken );
    BOOST_CHECK( ! senf::console::ArgumentTraits< 
                       senf::console::FlagCollection<TestEnum> >::singleToken );
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
