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

        void pushDirectory(std::vector<senf::console::Token> const & path)
            { os_ << "pushDirectory( " << senf::stringJoin(path, "/") << " )\n"; }
        void popDirectory()
            { os_ << "popDirectory()\n"; }

        void beginCommand(std::vector<senf::console::Token> const & command) 
            { os_ << "beginCommand( " << senf::stringJoin(command, "/") << " )\n"; }
        void endCommand() 
            { os_ << "endCommand()\n"; }
        
        void pushToken(senf::console::Token token)
            { os_ << "pushToken( " << token << " )\n"; }

        void builtin_cd(std::vector<senf::console::Token> const & path)
            { os_ << "builtin_cd( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_ls(std::vector<senf::console::Token> const & path)
            { os_ << "builtin_ls( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_exit()
            { os_ << "builtin_exit()\n"; }
        void builtin_help(std::vector<senf::console::Token> const & path)
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
        "                (a,b;c (huhu/{haha}))"
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
                       "beginCommand( Word('doo')/Word('bii')/Word('doo') )\n"
                       "pushToken( Word('arg') )\n"
                       "pushToken( Word('flab::blub') )\n"
                       "pushToken( Word('123.434>a') )\n"
                       "pushToken( ArgumentGroupOpen('(') )\n"
                       "pushToken( Word('a') )\n"
                       "pushToken( OtherPunctuation(',') )\n"
                       "pushToken( Word('b') )\n"
                       "pushToken( CommandTerminator(';') )\n"
                       "pushToken( Word('c') )\n"
                       "pushToken( ArgumentGroupOpen('(') )\n"
                       "pushToken( Word('huhu') )\n"
                       "pushToken( PathSeparator('/') )\n"
                       "pushToken( DirectoryGroupOpen('{') )\n"
                       "pushToken( Word('haha') )\n"
                       "pushToken( DirectoryGroupClose('}') )\n"
                       "pushToken( ArgumentGroupClose(')') )\n"
                       "pushToken( ArgumentGroupClose(')') )\n"
                       "pushToken( BasicString('foo\"bar') )\n"
                       "pushToken( HexString('\x01\x02\x03\x04') )\n"
                       "endCommand()\n"
                       "builtin_ls( None('')/Word('foo')/Word('bar') )\n"
                       "builtin_cd( None('')/Word('foo')/Word('bar') )\n"
                       "builtin_exit()\n"
                       "pushDirectory( Word('foo')/Word('bar')/None('') )\n"
                       "builtin_ls(  )\n"
                       "popDirectory()\n"
                       "builtin_help( None('')/Word('foo')/Word('bar') )\n" );
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

    senf::console::Token path[] = { 
        senf::console::Token(senf::console::Token::Word, "doo"), 
        senf::console::Token(senf::console::Token::Word, "bii"),
        senf::console::Token(senf::console::Token::Word, "doo")
    };

    BOOST_CHECK_EQUAL_COLLECTIONS( info.commandPath().begin(), info.commandPath().end(),
                                   path, path + sizeof(path)/sizeof(path[0]) );
    BOOST_CHECK_EQUAL( info.tokens().size(), 15u );

    char const * tokens[] = { "arg", 
                              "flab::blub", 
                              "123.434>a", 
                              "(", "a", ",", "b", ",", "c", "(", "huhu", ")", ")",
                              "foo\"bar",
                              "\x01\x02\x03\x04" };

    senf::console::ParseCommandInfo::argument_iterator args (info.arguments().begin());
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 1u );
    BOOST_CHECK_EQUAL( args->begin()->value(), tokens[0] );

    ++ args;
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 1u );
    BOOST_CHECK_EQUAL( args->begin()->value(), tokens[1] );
                         
    ++ args;
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 1u );
    BOOST_CHECK_EQUAL( args->begin()->value(), tokens[2] );

    ++ args;
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 8u );
    for (unsigned i (0); i<8; ++i)
        BOOST_CHECK_EQUAL( args->begin()[i].value(), tokens[4+i] );

    ++ args;
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 1u );
    BOOST_CHECK_EQUAL( args->begin()->value(), tokens[13] );

    ++ args;
    BOOST_REQUIRE( args != info.arguments().end() );
    BOOST_REQUIRE_EQUAL( args->size(), 1u );
    BOOST_CHECK_EQUAL( args->begin()->value(), tokens[14] );

    ++ args;
    BOOST_CHECK( args == info.arguments().end() );
}

namespace {
    void parseArgs(senf::console::ParseCommandInfo::ArgumentsRange const & args)
    {
        senf::console::CheckedArgumentIteratorWrapper arg (args);
        senf::console::ParseCommandInfo::TokensRange arg1 (*(arg++));
        senf::console::ParseCommandInfo::TokensRange arg2 (*(arg++));
    }
}

BOOST_AUTO_UNIT_TEST(checkedArgumentIterator)
{
    senf::console::CommandParser parser;

    BOOST_CHECK( parser.parse("foo a", &setInfo) );
    BOOST_CHECK_THROW( parseArgs(info.arguments()), senf::console::SyntaxErrorException );

    BOOST_CHECK( parser.parse("foo a b", &setInfo) );
    BOOST_CHECK_NO_THROW( parseArgs(info.arguments()) );

    BOOST_CHECK( parser.parse("foo a b c", &setInfo) );
    BOOST_CHECK_THROW( parseArgs(info.arguments()), senf::console::SyntaxErrorException );
    
    senf::console::CheckedArgumentIteratorWrapper arg (info.arguments());
    BOOST_CHECK( arg == info.arguments().begin() );
    BOOST_CHECK( arg != info.arguments().end() );
    BOOST_CHECK( arg );
    ++ arg;
    BOOST_CHECK( arg );
    arg.clear();
    BOOST_CHECK( arg.done() );

    senf::console::ParseCommandInfo::ArgumentIterator i (arg);
    BOOST_CHECK( i == info.arguments().end() );
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
