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
    \brief ParsedCommand.test unit tests */

//#include "ParsedCommand.test.hh"
//#include "ParsedCommand.test.ih"

// Custom includes
#include <sstream>
#include "ParsedCommand.hh"
#include "Executor.hh"
#include "Parse.hh"
#include "ScopedDirectory.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    int cb1(int a, double b) { return int(a+b); }
    double cb2() { return 1.2; }
    void cb3(int) {}
    std::string cb4(std::ostream & os) { os << "text\n"; return "value"; }
    void cb5(std::ostream & os, int v) { os << "Value: " << v << "\n"; }
}

BOOST_AUTO_UNIT_TEST(parsedCommand)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    {
        std::stringstream ss;
        dir.add("cb1", &cb1);
        parser.parse("test/cb1 2 3.2", 
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "5\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb2", &cb2);
        parser.parse("test/cb2",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "1.2\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb3", &cb3);
        parser.parse("test/cb3 234",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "" );
    }

    {
        std::stringstream ss;
        dir.add("cb4", &cb4);
        parser.parse("test/cb4",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "text\n" "value\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb5", &cb5);
        parser.parse("test/cb5 1234",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "Value: 1234\n" );
    }

    {
        std::stringstream ss;

        BOOST_CHECK_THROW( 
            parser.parse("test/cb1 2 3.2 foo", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );

        BOOST_CHECK_THROW(
            parser.parse("test/cb1 2", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );

        BOOST_CHECK_THROW(
            parser.parse("test/cb1 2 foo", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );
    }
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
