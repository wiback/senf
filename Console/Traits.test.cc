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
    \brief Traits.test unit tests */

//#include "Traits.test.hh"
//#include "Traits.test.ih"

// Custom includes
#include "Traits.hh"
#include "ParsedCommand.hh"
#include "Executor.hh"
#include "Parse.hh"
#include "ScopedDirectory.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    enum TestEnum { Foo, Bar };
    SENF_CONSOLE_REGISTER_ENUM( TestEnum, (Foo)(Bar) );

    TestEnum test (TestEnum value) { return value; }

    struct TestClass {
        enum MemberEnum { MemberFoo, MemberBar };
        static MemberEnum test (MemberEnum value) { return value; }
    };
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( TestClass, MemberEnum, (MemberFoo)(MemberBar) );
    
}

BOOST_AUTO_UNIT_TEST(enumSupport)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    dir.add("test",&test);
    
    std::stringstream ss;
    BOOST_CHECK_NO_THROW(
        parser.parse("test/test Foo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "Foo\n" );

    ss.str("");
    BOOST_CHECK_NO_THROW(
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
    BOOST_CHECK_NO_THROW(
        parser.parse("test/member MemberFoo",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "MemberFoo\n" );

    ss.str("");
    BOOST_CHECK_NO_THROW(
        parser.parse("test/member MemberBar",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
    BOOST_CHECK_EQUAL( ss.str(), "MemberBar\n" );
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
