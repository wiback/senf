// $Id$
//
// Copyright (C) 2009 
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
    \brief Utility.test unit tests */

//#include "Utility.test.hh"
//#include "Utility.test.ih"

// Custom includes
#include "Utility.hh"
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

    senf::console::FlagCollection<TestEnum> collectionTest(
        senf::console::FlagCollection<TestEnum> flags) { return flags; }

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
