//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Parse unit tests */

// #define BOOST_SPIRIT_DEBUG
// #define BOOST_SPIRIT_DEBUG_TRACENODE 0

// Custom includes
#include <sstream>
#include <senf/Utils/String.hh>
#include "Console.hh"
#include "Parse.ih"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    struct TestParseDispatcher
    {
        TestParseDispatcher(std::ostream & os) : os_ (os) {}

        std::ostream & os_;

        void pushDirectory()
            { os_ << "pushDirectory()\n"; }
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
        void builtin_ll(std::vector<senf::console::Token> const & path)
            { os_ << "builtin_ll( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_lr(std::vector<senf::console::Token> const & path)
            { os_ << "builtin_lr( " << senf::stringJoin(path, "/") << " )\n"; }
        void builtin_echo()
            { os_ << "builtin_echo()\n"; }
        void builtin_exit()
            { os_ << "builtin_exit()\n"; }
        void builtin_help(std::vector<senf::console::Token> const & path)
            { os_ << "builtin_help( " << senf::stringJoin(path, "/") << " )\n"; }
    };
}

SENF_AUTO_TEST_CASE(commandGrammar)
{
    senf::console::detail::CommandGrammar<TestParseDispatcher>::Context context;
    std::stringstream ss;
    TestParseDispatcher dispatcher (ss);

    typedef senf::console::detail::CommandGrammar<TestParseDispatcher> Grammar;
    Grammar grammar (dispatcher, context);

    {
        static char text[] =
            "# Comment\n"
            "doo / bii / // doo arg"
            "                flab::blub"
            "                123.434>a"
            "                (a,b;c (huhu/{haha}))"
            "                \"foo\\\"bar\" #\n"
            "                x\"01 02 # Inner comment\n"
            "                   0304\";";

        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
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
                           "endCommand()\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "ls //foo/\"bar\";",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "builtin_ls( None('')/Word('foo')/BasicString('bar') )\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "lr /foo/bar;",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "builtin_lr( None('')/Word('foo')/Word('bar') )\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "cd /foo/bar;",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "builtin_cd( None('')/Word('foo')/Word('bar') )\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "exit;",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "builtin_exit()\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "foo/\"bar baz\"// {",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(),
                           "beginCommand( Word('foo')/BasicString('bar baz')/None('') )\n"
                           "pushDirectory()\n"
                           "endCommand()\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "}",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "popDirectory()\n" );
    }

    {
        ss.str("");
        BOOST_CHECK( senf::console::detail::boost_spirit::parse(
                         "help /foo/bar",
                         grammar.use_parser<Grammar::CommandParser>(),
                         grammar.use_parser<Grammar::SkipParser>() ) . full );
        BOOST_CHECK_EQUAL( ss.str(), "builtin_help( None('')/Word('foo')/Word('bar') )\n" );
    }
}

namespace {
    std::vector<senf::console::ParseCommandInfo> commands;
    void setInfo(senf::console::ParseCommandInfo const & i)
    { commands.push_back(i); }
}

SENF_AUTO_TEST_CASE(commandParser)
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
        "                   0304\";"
        "ls /foo/bar; ";

    SENF_CHECK_NO_THROW( parser.parse(text, &setInfo) );
    BOOST_CHECK_EQUAL( commands.size(), 2u );

    {
        senf::console::ParseCommandInfo const & info (commands.front());

        senf::console::Token path[] = {
            senf::console::Token(senf::console::Token::Word, "doo"),
            senf::console::Token(senf::console::Token::Word, "bii"),
            senf::console::Token(senf::console::Token::Word, "doo")
        };

        BOOST_CHECK_EQUAL_COLLECTIONS( info.commandPath().begin(), info.commandPath().end(),
                                       path, path + sizeof(path)/sizeof(path[0]) );
        BOOST_CHECK_EQUAL( boost::next(info.commandPath().begin())->index(), 16u );
        BOOST_CHECK_EQUAL( unsigned(info.tokens().size()), 15u );

        char const * tokens[] = { "arg",
                                  "flab::blub",
                                  "123.434>a",
                                  "(", "a", ",", "b", ",", "c", "(", "huhu", ")", ")",
                                  "foo\"bar",
                                  "\x01\x02\x03\x04" };

        senf::console::ParseCommandInfo::argument_iterator args (info.arguments().begin());
        BOOST_REQUIRE( args != info.arguments().end() );
        BOOST_REQUIRE_EQUAL( unsigned(args->size()), 1u );
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
        BOOST_CHECK_EQUAL( info.tokens().begin()[3].index(), 96u );
        BOOST_CHECK_EQUAL( info.tokens().begin()[5].index(), 98u );
        BOOST_CHECK_EQUAL( info.tokens().begin()[12].index(), 109u );

        ++ args;
        BOOST_REQUIRE( args != info.arguments().end() );
        BOOST_REQUIRE_EQUAL( args->size(), 1u );
        BOOST_CHECK_EQUAL( args->begin()->value(), tokens[13] );
        BOOST_CHECK_EQUAL( args->begin()->index(), 126u );

        ++ args;
        BOOST_REQUIRE( args != info.arguments().end() );
        BOOST_REQUIRE_EQUAL( args->size(), 1u );
        BOOST_CHECK_EQUAL( args->begin()->value(), tokens[14] );

        ++ args;
        BOOST_CHECK( args == info.arguments().end() );
    }

    commands.clear();
}

namespace {
    void parseArgs(senf::console::ParseCommandInfo::ArgumentsRange const & args)
    {
        senf::console::CheckedArgumentIteratorWrapper arg (args);
        senf::console::ParseCommandInfo::TokensRange arg1 (*(arg++));
        senf::console::ParseCommandInfo::TokensRange arg2 (*(arg++));
        BOOST_CHECK( arg1 );
        BOOST_CHECK( arg2 );
    }
}

SENF_AUTO_TEST_CASE(checkedArgumentIterator)
{
    senf::console::CommandParser parser;

    SENF_CHECK_NO_THROW( parser.parse("foo a", &setInfo) );
    BOOST_CHECK_THROW( parseArgs(commands.back().arguments()),
                       senf::console::SyntaxErrorException );

    SENF_CHECK_NO_THROW( parser.parse("foo a b", &setInfo) );
    SENF_CHECK_NO_THROW( parseArgs(commands.back().arguments()) );

    SENF_CHECK_NO_THROW( parser.parse("foo a b c", &setInfo) );
    BOOST_CHECK_THROW( parseArgs(commands.back().arguments()),
                       senf::console::SyntaxErrorException );

    senf::console::CheckedArgumentIteratorWrapper arg (commands.back().arguments());
    BOOST_CHECK( arg == commands.back().arguments().begin() );
    BOOST_CHECK( arg != commands.back().arguments().end() );
    BOOST_CHECK( arg );
    ++ arg;
    BOOST_CHECK( arg );
    arg.clear();
    BOOST_CHECK( arg.done() );

    senf::console::ParseCommandInfo::ArgumentIterator i (arg);
    BOOST_CHECK( i == commands.back().arguments().end() );

    commands.clear();
}

SENF_AUTO_TEST_CASE(parseIncremental)
{
    senf::console::CommandParser parser;

    BOOST_CHECK_EQUAL( parser.parseIncremental("foo a", &setInfo), 0u );
    BOOST_CHECK_EQUAL( parser.parseIncremental("foo a; cd", &setInfo), 7u );
    BOOST_CHECK_EQUAL( parser.parseIncremental("foo a; cd /bar", &setInfo), 7u );
    BOOST_CHECK_EQUAL( parser.parseIncremental("foo a; cd /bar; ", &setInfo), 16u );
    BOOST_CHECK_EQUAL( parser.parseIncremental(" ", &setInfo), 1u );
    BOOST_CHECK_EQUAL( commands.size(), 4u );

    commands.clear();
}

namespace {
    std::string parseErrorMessage(std::string const & msg)
    {
        std::string::size_type i (msg.find("-- \n"));
        return i == std::string::npos ? msg : msg.substr(i+4);
    }
}

SENF_AUTO_TEST_CASE(parseExceptions)
{
    senf::console::CommandParser parser;
    std::string msg;

#   define CheckParseEx(c, e)                                                                     \
        commands.clear();                                                                         \
        msg.clear();                                                                              \
        try { parser.parse(c, &setInfo); }                                                        \
        catch (std::exception & ex) { msg = parseErrorMessage(ex.what()); }                       \
        BOOST_CHECK_EQUAL( msg, e )

    CheckParseEx( "/foo/bar;\n  ()", "path expected\nat <unknown>:2:3" );
    CheckParseEx( "cd /foo/bar foo/bar", "end of statement expected\nat <unknown>:1:13" );
    CheckParseEx( "/foo/bar foo /", "end of statement expected\nat <unknown>:1:14" );
    CheckParseEx( "cd (foo)", "path expected\nat <unknown>:1:4" );
    CheckParseEx( "/foo/bar \"string", "'\"' expected\nat <unknown>:1:17" );
    CheckParseEx( "/foo/bar x\"hi\"", "'\"' expected\nat <unknown>:1:12" );
    CheckParseEx( "/foo/bar (", "')' expected\nat <unknown>:1:11" );
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
