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
    \brief Parse.test unit tests */

//#include "Parse.test.hh"
//#include "Parse.test.ih"

// #define BOOST_SPIRIT_DEBUG
// #define BOOST_SPIRIT_DEBUG_TRACENODE 0

// Custom includes
#include <sstream>
#include "Parse.hh"
#include "Parse.ih"
#include "../Utils/String.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace 
{
    struct TestParseDispatcher 
    {
        TestParseDispatcher(std::ostream & os) : os_ (os) {}

        std::ostream & os_;

        void pushDirectory(std::vector<std::string> const & path)
            { os_ << "pushDirectory( " << senf::stringJoin(path,"/") << " )\n"; }
        void popDirectory()
            { os_ << "popDirectory()\n"; }

        void beginCommand(std::vector<std::string> const & command) 
            { os_ << "beginCommand( " << senf::stringJoin(command, "/") << " )\n"; }
        void endCommand() 
            { os_ << "endCommand()\n"; }
        
        void pushArgument(std::string const & argument)
            { os_ << "pushArgument( " << argument << " )\n"; }
        void openGroup()
            { os_ << "openGroup()\n"; }
        void closeGroup()
            { os_ << "closeGroup()\n"; }
        void pushPunctuation(std::string const & token)
            { os_ << "pushPunctuation( " << token << " )\n"; }
        void pushWord(std::string const & token)
            { os_ << "pushWord( " << token << " )\n"; }

        void builtin_cd(std::vector<std::string> const & path)
            { os_ << "builtin_cd( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_ls(std::vector<std::string> const & path)
            { os_ << "builtin_ls( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_exit()
            { os_ << "builtin_exit()\n"; }
        void builtin_help(std::vector<std::string> const & path)
            { os_ << "builtin_help( " << senf::stringJoin(path, "/") << " )\n"; }
    };
}

BOOST_AUTO_UNIT_TEST(commandGrammar)
{
    senf::console::detail::CommandGrammar<TestParseDispatcher>::Context context;
    std::stringstream ss;
    TestParseDispatcher dispatcher (ss);
    
    typedef senf::console::detail::CommandGrammar<TestParseDispatcher> Grammar;
    Grammar grammar (dispatcher, context);

    char text[] = 
        "# Comment\n"
        "doo / bii / doo arg"
        "                flab::blub"
        "                123.434>a"
        "                (a,b,c (huhu))"
        "                \"foo\\\"bar\" #\n"
        "                x\"01 02 # Inner comment\n"
        "                   0304\";"
        "ls /foo/bar;"
        "cd /foo/bar;"
        "exit;"
        "foo/bar/ { ls; }"
        "help /foo/bar";

    BOOST_CHECK( boost::spirit::parse( 
                     text, 
                     grammar.use_parser<Grammar::CommandParser>(), 
                     grammar.use_parser<Grammar::SkipParser>() ) . full );
    BOOST_CHECK_EQUAL( ss.str(), 
                       "beginCommand( doo/bii/doo )\n"
                       "pushArgument( arg )\n"
                       "pushArgument( flab::blub )\n"
                       "pushArgument( 123.434>a )\n"
                       "openGroup()\n"
                       "pushWord( a )\n"
                       "pushPunctuation( , )\n"
                       "pushWord( b )\n"
                       "pushPunctuation( , )\n"
                       "pushWord( c )\n"
                       "pushPunctuation( ( )\n"
                       "pushWord( huhu )\n"
                       "pushPunctuation( ) )\n"
                       "closeGroup()\n"
                       "pushArgument( foo\"bar )\n"
                       "pushArgument( \x01\x02\x03\x04 )\n"
                       "endCommand()\n"
                       "builtin_ls( /foo/bar )\n"
                       "builtin_cd( /foo/bar )\n"
                       "builtin_exit()\n"
                       "pushDirectory( foo/bar/ )\n"
                       "builtin_ls(  )\n"
                       "popDirectory()\n"
                       "builtin_help( /foo/bar )\n" );
}

namespace {
    senf::console::ParseCommandInfo info;
    void setInfo(senf::console::ParseCommandInfo const & i)
    { info = i; }
}

BOOST_AUTO_UNIT_TEST(commandParser)
{
    senf::console::CommandParser parser;

    char const text[] = 
        "# Comment\n"
        "doo / bii / doo arg"
        "                flab::blub"
        "                123.434>a"
        "                (a,b,c (huhu))"
        "                \"foo\\\"bar\" #\n"
        "                x\"01 02 # Inner comment\n"
        "                   0304\"";

    BOOST_CHECK( parser.parse(text, &setInfo) );

    char const * path[] = { "doo", "bii", "doo" };

    BOOST_CHECK_EQUAL_COLLECTIONS( info.commandPath().begin(), info.commandPath().end(),
                                   path, path + sizeof(path)/sizeof(path[0]) );
    BOOST_REQUIRE_EQUAL( info.arguments().size(), 6u );
    BOOST_REQUIRE_EQUAL( info.tokens().size(), 13u );

    char const * tokens[] = { "arg", 
                              "flab::blub", 
                              "123.434>a", 
                              "a", ",", "b", ",", "c", "(", "huhu", ")",
                              "foo\"bar",
                              "\x01\x02\x03\x04" };

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[0].size(), 1u );
    BOOST_CHECK_EQUAL( info.arguments().begin()[0].begin()->value(), tokens[0] );

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[1].size(), 1u );
    BOOST_CHECK_EQUAL( info.arguments().begin()[1].begin()->value(), tokens[1] );

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[2].size(), 1u );
    BOOST_CHECK_EQUAL( info.arguments().begin()[2].begin()->value(), tokens[2] );

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[3].size(), 8u );
    for (unsigned i (0); i<8; ++i)
        BOOST_CHECK_EQUAL( info.arguments().begin()[3].begin()[i].value(), tokens[3+i] );

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[4].size(), 1u );
    BOOST_CHECK_EQUAL( info.arguments().begin()[4].begin()->value(), tokens[11] );

    BOOST_REQUIRE_EQUAL( info.arguments().begin()[5].size(), 1u );
    BOOST_CHECK_EQUAL( info.arguments().begin()[5].begin()->value(), tokens[12] );
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
