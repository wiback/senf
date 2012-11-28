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
    \brief Parse public header */

#ifndef HH_SENF_Utils_Console_Parse_
#define HH_SENF_Utils_Console_Parse_ 1

/** \defgroup console_parser The parser

    The console/config library defines a simple language used to interact with the console or to
    configure the application.  The parser is not concerned about interpreting commands or
    arguments, checking that a command exists or managing directories. The parser just takes the
    input and parses it.

    \autotoc

    \section console_language The Language

    The config/console language is used in configuration files and interactively at the
    console. Some features of the language are more useful in config files, others at the
    interactive console but the language is the same in both cases.

    Let's start with a sample of the config/console language. The following is written as a
    configuration file
    \code
    # My someserver configuration file

    /server/port 1234;

    /logger/targets {
        console {
            accept senf::log::Debug IMPORTANT;
            accept server::ServerLog CRITICAL;
        }

        provide serverlog senf::log::FileTarget "/var/log/server.log";
        serverlog {
            reject senf::log::Debug senf::Console::Server NOTICE;
            accept senf::log::Debug NOTICE;
            accept server::ServerLog;
        }
    }

    /server/stuffing (UDPPacket x"01 02 03 04");
    /server/allow_hosts 10.1.2.3               # our internal server
                        10.2.3.4 10.4.3.5      # client workstations
        ;

    /help/infoUrl "http://senf.j32.de/src/doc";
    \endcode

    The interactive syntax is the same with some notes:
    \li All commands must be complete on a single line. This includes grouping constructs which must
        be closed on the same line they are opened.
    \li The last ';' is optional. However, multiple commands may be entered on a single line when
        they are separated by ';'.
    \li An empty line on the interactive console will repeat the last command.

    The language consists of a small number of syntactic entities:

    \subsection console_special_chars Special characters

    These are characters, which have a special meaning. Some are used internally, others are just
    returned as punctuation tokens

    <table class="senf">
        <tr><td>#</td><td>Comments are marked with '#' and continue to the end of the line</td></tr>
        <tr><td>/</td><td>path component separator</td></tr>
        <tr><td>( )</td><td>argument grouping</td></tr>
        <tr><td>{ }</td><td>directory grouping</td></tr>
        <tr><td>;</td><td>command terminator</td></tr>
        <tr><td>, =</td><td>punctuation tokens</td></tr>
    </table>

    \subsection console_basic Basic elements

    A <b>word</b> is \e any sequence of consecutive characters which does not include any special
    character. Examples for words are thus
    <pre>
    12.34
    jens@fokus.fraunhofer.de
    eth0
    1>2
    </pre>

    The following are \e not valid words:
    <pre>
    a/b/c
    a,b
    </pre>

    A <b>string literal</b> is just that: A double-quoted string (C/C++ style) possibly with
    embedded escape chars:
    <pre>
    "\"foo\nbar\""
    "\x04test"
    </pre>

    A <b>hex-string literal</b> is used to represent binary data. It looks like a string which has
    only hexadecimal bytes or whitespace as contents (comments and newlines are Ok when not read
    from the interactive console)
    <pre>
    x"01 02 03 0405"
    x"01 02   # ID header
      0405    # payload
      "
    </pre>

    A <b>token</b> is a \e word, \e string or \e hex-string, or a single special character (that's
    true, any special character is allowed as a token). '(' and ')' must be properly nested.

    A <b>path</b> is a sequence of \e words separated by '/' (and optional whitespace). A path may
    have an optional initial and/or a terminating '/'.
    <pre>
    a/b/c
    foo / bar /
    /server
    </pre>

    \subsection console_statements Statements

    There are several types of statements:
    \li The bulk of all statements are \e path statements
    \li There are some \e built-in statements which are mostly useful at the interactive console
    \li A special form of statement is the <em>directory group</em>

    A <b>path</b> statement consists of a (possibly relative) path followed by any number of
    arguments and terminated with a ';' (or end-of-input)
    <pre>
    /path/to/command arg1 "arg2" (complex=(1 2) another) ;
    </pre>
    Every argument is either
    \li A single word, string or hex-string
    \li or a parenthesized list of tokens.

    So above command has three arguments: 'arg1', 'arg2' (a single token each) and one argument with
    the 7 tokens 'complex', '=', '(', '1', '2', ')', 'another'. The interpretation of the arguments
    is completely up to the command.

    A <b>built-in</b> statement is one of

    <table class="senf">
    <tr><td>\c cd \e path</td><td>Change current directory</td></tr>
    <tr><td>\c ls [ \e path ]</td><td>List contents of \e path or current directory</td></tr>
    <tr><td>\c exit</td><td>Exit interactive console</td></tr>
    <tr><td>\c help [ \e path ]</td><td>Show help for \e path or current directory</td></tr>
    </table>

    A <b>directory group</b> statement is a block of statements all executed relatively to a fixed
    directory.
    <pre>
    /some/path {
        statement ;
        . . .
    }
    </pre>
    At the beginning of the block, the current directory is saved and the directory is changed to
    the given directory. All commands are executed and at the end of the block, the saved directory
    is restored.

    \section console_parse_api The parser API

    The senf::console::CommandParser is responsible for taking text input and turning it into a
    sequence of senf::console::ParseCommandInfo structures. The structures are returned by passing
    them successively to a callback function.

    Every statement is returned as a senf::console::ParseCommandInfo instance. Directory groups are
    handled specially: They are divided into two special built-in commands called PUSHD and POPD.
 */

// Custom includes
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/function.hpp>
#include <senf/Utils/safe_bool.hh>
#include <senf/Utils/Exception.hh>

//#include "Parse.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    namespace detail {
        struct FilePositionWithIndex;
        struct ParserAccess;
        template <class ParameterType>
        struct ArgumentInfo;
    }

#   define SENF_CONSOLE_PARSE_FRIEND( type )                                                                    \
        friend void senf_console_parse_argument(senf::console::ParseCommandInfo::TokensRange const &, type &);  \
        friend struct senf::console::detail::ArgumentInfo<type>;


    /** \brief Single argument token

        All command arguments are split into tokens by the parser. Each token is returned as an
        Token instance.

        \ingroup console_parser
      */
    class Token
    {
    public:
        enum TokenType {
            None                = 0,
            PathSeparator       = 0x0001, // '/'
            ArgumentGroupOpen   = 0x0002, // '('
            ArgumentGroupClose  = 0x0004, // ')'
            DirectoryGroupOpen  = 0x0008, // '{'
            DirectoryGroupClose = 0x0010, // '}'
            CommandTerminator   = 0x0020, // ';'
            OtherPunctuation    = 0x0040,
            BasicString         = 0x0080,
            HexString           = 0x0100,
            Word                = 0x0200
        };

        enum TokenGroup {
            ArgumentGrouper     = ArgumentGroupOpen
                                | ArgumentGroupClose,

            DirectoryGrouper    = DirectoryGroupOpen
                                | DirectoryGroupClose,

            Punctuation         = DirectoryGroupOpen
                                | DirectoryGroupClose
                                | PathSeparator
                                | CommandTerminator
                                | OtherPunctuation,

            String              = BasicString
                                | HexString,

            SimpleArgument      = Word
                                | BasicString
                                | HexString
        };

        Token();                        ///< Create empty token
        Token(TokenType type, std::string token);
                                        ///< Create token with given type and value
        Token(TokenType type, std::string token, detail::FilePositionWithIndex const & pos);
                                        ///< Create token with given type and value


        std::string const & value() const; ///< String value of token
                                        /**< This value is properly unquoted */

        TokenType type() const;         ///< Token type

        unsigned line() const;          ///< Line number of token in source
        unsigned column() const;        ///< Column number of token in source
        unsigned index() const;         ///< Index (char count) of token in source

        bool is(unsigned tokens) const; ///< Check, whether tokens type matches \a tokens
                                        /**< \a tokens is a bit-mask of token types to check. */

        bool operator==(Token const & other) const;
        bool operator!=(Token const & other) const;

    protected:

    private:
        TokenType type_;
        std::string token_;
        unsigned line_;
        unsigned column_;
        unsigned index_;
    };

    std::ostream & operator<<(std::ostream & os, Token const & token);

    /** \brief Create a \c None token
        \related Token */
    Token NoneToken();

    /** \brief Create a \c PathSeparator ['/'] token
        \related Token */
    Token PathSeparatorToken();

    /** \brief Create an \c ArgumentGroupOpen ['('] token
        \related Token */
    Token ArgumentGroupOpenToken();

    /** \brief Create a \c ArgumentGroupClose [')'] token
        \related Token */
    Token ArgumentGroupCloseToken();

    /** \brief Create a \c DirectoryGroupOpen ['{'] token
        \related Token */
    Token DirectoryGroupOpenToken();

    /** \brief Create a \c DirectoryGroupClose ['}'] token
        \related Token */
    Token DirectoryGroupCloseToken();

    /** \brief Create a \c CommandTerminator [';'] token
        \related Token */
    Token CommandTerminatorToken();

    /** \brief Create a \c OtherPunctuation ['=', ','] token with the given \a value
        \related Token */
    Token OtherPunctuationToken(std::string const & value);

    /** \brief Create a \c BasicString token with the given \a value
        \related Token */
    Token BasicStringToken(std::string const & value);

    /** \brief Create a \c HexString token with the given \a value
        \related Token */
    Token HexStringToken(std::string const & value);

    /** \brief Create a \c Word token with the given \a value
        \related Token */
    Token WordToken(std::string const & value);

    /** \brief Single parsed console command

        Every command parsed is returned in a ParseCommandInfo instance. This information is purely
        taken from the parser, no semantic information is attached at this point, the config/console
        node tree is not involved in any way. ParseCommandInfo consist of

        \li the type of command: built-in or normal command represented by a possibly relative path
            into the command tree.
        \li the command
        \li the arguments. Every argument consists of a range of Token instances.

        \ingroup console_parser
      */
    class ParseCommandInfo
    {
        typedef std::vector<Token> Tokens;
        typedef std::vector<std::string> CommandPath;

    public:
        class ArgumentIterator;

        typedef CommandPath::const_iterator path_iterator;
        typedef Tokens::const_iterator token_iterator;
        typedef ArgumentIterator argument_iterator;
        typedef Tokens::size_type size_type;

        typedef boost::iterator_range<path_iterator> CommandPathRange;
        typedef boost::iterator_range<argument_iterator> ArgumentsRange;
        typedef boost::iterator_range<token_iterator> TokensRange;

        enum BuiltinCommand { NoBuiltin,
                              BuiltinCD,
                              BuiltinLS,
                              BuiltinLL,
                              BuiltinLR,
                              BuiltinPUSHD,
                              BuiltinPOPD,
                              BuiltinEXIT,
                              BuiltinHELP,
                              BuiltinECHO };

        ParseCommandInfo();

        BuiltinCommand builtin() const; ///< Command type
                                        /**< \returns \c NoBuiltin, if the command is an ordinary
                                             command, otherwise the id of the built-in command */
        TokensRange commandPath() const; ///< Command path
                                        /**< This is the path to the command if it is not a built-in
                                             command. Every element of the returned range
                                             constitutes one path element. If the first element is
                                             empty, the path is an absolute path, otherwise it is
                                             relative. If the last element is an empty string, the
                                             path ends with a '/' char. */
        ArgumentsRange arguments() const; ///< Command arguments
                                        /**< The returned range contains one TokensRange for each
                                             argument. */
        TokensRange tokens() const;     ///< All argument tokens
                                        /**< The returned range contains \e all argument tokens in a
                                             single range not divided into separate arguments. */

        void clear();                   ///< Clear all data members
        bool empty();                   ///< \c true, if the data is empty

        void builtin(BuiltinCommand builtin); ///< Assign builtin command
        void command(std::vector<Token> & commandPath); ///< Assign non-builtin command

        void addToken(Token const & token); ///< Add argument token
                                        /**< You \e must ensure, that the resulting argument tokens
                                             are properly nested regarding '()' groups, otherwise
                                             interpreting arguments using the arguments() call will
                                             crash the program. */

    protected:

    private:
        struct MakeRange;

        std::vector<Token> commandPath_;
        BuiltinCommand builtin_;
        Tokens tokens_;
    };

    /** \brief Iterator parsing argument groups

        This special iterator parses a token range returned by the parser into argument ranges. An
        argument range is either a single token or it is a range of tokens enclosed in matching
        parenthesis. The ParseCommandInfo::arguments() uses this iterator type. To recursively parse
        complex arguments, you can however use this iterator to divide a multi-token argument into
        further argument groups (e.g. to parse a list or vector of items).

        This iterator is a bidirectional iterator \e not a random access iterator.
     */
    class ParseCommandInfo::ArgumentIterator
        : public boost::iterator_facade< ParseCommandInfo::ArgumentIterator,
                                         ParseCommandInfo::TokensRange,
                                         boost::bidirectional_traversal_tag,
                                         ParseCommandInfo::TokensRange >
    {
    public:
        ArgumentIterator();
        explicit ArgumentIterator(ParseCommandInfo::TokensRange::iterator i);

    private:
        reference dereference() const;
        bool equal(ArgumentIterator const & other) const;
        void increment();
        void decrement();

        mutable ParseCommandInfo::TokensRange::iterator b_;
        mutable ParseCommandInfo::TokensRange::iterator e_;

        void setRange() const;

        friend class boost::iterator_core_access;
        friend class ParseCommandInfo;
    };

    /**  \brief Syntax error parsing command arguments exception

        All errors while parsing the arguments of a command must be signaled by throwing an instance
        of SyntaxErrorException. This is important, so command overloading works.
     */
    struct SyntaxErrorException : public senf::Exception
    { explicit SyntaxErrorException(std::string const & msg = "syntax error")
          : senf::Exception(msg) {} };

    /** \brief Wrapper checking argument iterator access for validity

        CheckedArgumentIteratorWrapper is a wrapper around a range of arguments parsed using the
        ParseCommandInfo::ArgumentIterator. It is used to parse arguments either in a command
        (registered with manual argument parsing) or when defining a custom parser.
        \code
        void fn(std::ostream & out, senf::console::ParseCommandInfo command)
        {
            std:;string arg1;
            unsigned arg2 (0);

            {
                senf::console::CheckedArgumentIteratorWrapper arg (command.arguments());
                senf::console::parse( *(arg++), arg1 );
                senf::console::parse( *(arg++), arg2 );
            }

            // ...
        }
        \endcode

        To use the wrapper, you must ensure that:
        \li You increment the iterator \e past all arguments you parse. The iterator must point to
            the end of the range when parsing is complete.
        \li The iterator wrapper is destroyed after parsing but before executing the command itself
            begins.

        Accessing a non-existent argument or failing to parse all arguments will raise a
        senf::console::SyntaxErrorException.

        \see \ref console_args_custom "Example customer parser"
      */
    class CheckedArgumentIteratorWrapper
        : boost::noncopyable,
          public boost::iterator_facade< CheckedArgumentIteratorWrapper,
                                         ParseCommandInfo::TokensRange,
                                         boost::forward_traversal_tag,
                                         ParseCommandInfo::TokensRange >,
          public senf::safe_bool<CheckedArgumentIteratorWrapper>

    {
        typedef boost::iterator_facade< CheckedArgumentIteratorWrapper,
                                        ParseCommandInfo::TokensRange,
                                        boost::forward_traversal_tag,
                                        ParseCommandInfo::TokensRange > IteratorFacade;

    public:
        explicit CheckedArgumentIteratorWrapper(
            ParseCommandInfo::ArgumentsRange const & range,
            std::string const & msg = "invalid number of arguments");
                                        ///< Make wrapper from ArgumentsRange
                                        /**< This constructs a wrapper from a
                                             ParseCommandInfo::ArgumentsRange.
                                             \param[in] range Range of arguments to parse
                                             \param[in] msg Error message */
        explicit CheckedArgumentIteratorWrapper(
            ParseCommandInfo::TokensRange const & range,
            std::string const & msg = "invalid number of arguments");
                                        ///< Make wrapper from TokensRange
                                        /**< This constructs a wrapper from a
                                             ParseCommandInfo::TokensRange. The TokensRange is first
                                             converted into an ParseCommandInfo::ArgumentsRange
                                             which is then wrapped.
                                             \param[in] range Range of tokens to  parse
                                             \param[in] msg Error message */

        ~CheckedArgumentIteratorWrapper(); ///< Check, if all arguments are parsed
                                        /**< The destructor validates, that all arguments are parsed
                                             correctly when leaving the scope, in which the wrapper
                                             is instantiated normally (not by an exception).

                                             \warning This destructor will throw a
                                             SyntaxErrorException, if not all arguments are parsed
                                             and when no other exception is in progress. */

        operator ParseCommandInfo::ArgumentIterator();
                                        ///< Use wrapper as ParseCommandInfo::ArgumentIterator

        bool boolean_test() const;      ///< \c true, if more arguments are available
        bool done() const;              ///< \c true, if all arguments are parsed

        void clear();                   ///< Set range empty
                                        /**< This call will point the current iterator to the end of
                                             the tokens range.
                                             \post done() == \c true; */

        bool operator==(ParseCommandInfo::ArgumentIterator const & other) const;
                                        ///< Compare wrapper against ArgumentIterator
        bool operator!=(ParseCommandInfo::ArgumentIterator const & other) const;
                                        ///< Compare wrapper against ArgumentIterator

        using IteratorFacade::operator++;
        ParseCommandInfo::ArgumentIterator operator++(int);

    private:
        reference dereference() const;
        void increment();

        ParseCommandInfo::ArgumentIterator i_;
        ParseCommandInfo::ArgumentIterator e_;
        std::string msg_;

        friend class boost::iterator_core_access;
    };

    /**< \brief Output ParseCommandInfo instance
         \related ParseCommandInfo
      */
    std::ostream & operator<<(std::ostream & stream, ParseCommandInfo const & info);

    /** \brief Parse commands

        This class implements a parser for the console/config language. It supports parsing strings
        as well as files. For every parsed command, a callback function is called.

        \implementation The implementation is based on Boost.Spirit. See the file \ref Parse.ih for
            the formal language grammar.

        \implementation Parsing an arbitrary iostream is not supported since arbitrary streams are
            not seekable. If this is needed, it can however be provided using stream iterators and
            some special iterator adaptors from Boost.Spirit. However, the amount of backtracking
            needs to be analyzed before this is viable.

        \ingroup console_parser
      */
    class CommandParser
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (ParseCommandInfo const &)> Callback;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        CommandParser();
        ~CommandParser();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void parse(std::string const & command, Callback cb); ///< Parse string
        void parseFile(std::string const & filename, Callback cb); ///< Parse file
                                        /**< \throws SystemException if the file cannot be
                                             read. */

        void parseArguments(std::string const & arguments, ParseCommandInfo & info);
                                        ///< Parse \a arguments
                                        /**< parseArguments() parses the string \a arguments which
                                             contains arbitrary command arguments (without the name
                                             of the command). The argument tokens are written into
                                             \a info. */

        void parsePath(std::string const & path, ParseCommandInfo & info);
                                        ///< Parse \a path
                                        /**< parsePath() parses the string \a path as an arbitrary
                                             command path. The result is written into \a info. */

        std::string::size_type parseIncremental(std::string const & commands, Callback cb);
                                        ///< Incremental parse
                                        /**< An incremental parse will parse all complete statements
                                             in \a commands. parseIncremental() will return the
                                             number of characters successfully parsed from \a
                                             commands.

                                             \note The incremental parser \e requires all statements
                                                 to be terminated explicitly. This means, that the
                                                 last ';' is \e not optional in this case. */

        static bool isSpecialChar(char ch); ///< Check, if \a ch is a special character
        static bool isPunctuationChar(char ch); ///< Check, if \a ch is a punctuation character
        static bool isSpaceChar(char ch); ///< Check, if \a ch is a space character
        static bool isInvalidChar(char ch); ///< Check, if \a ch is an invalid character
        static bool isWordChar(char ch); ///< Check, if \a ch is a word character

        /** \brief Exception thrown when the parser detects an error */
        struct ParserErrorException : public SyntaxErrorException
        { explicit ParserErrorException(std::string const & msg) : SyntaxErrorException(msg) {} };

    private:
        struct Impl;
        struct SetIncremental;

        template <class Iterator>
        Iterator parseLoop(Iterator b, Iterator e, std::string const & source, Callback cb);

        Impl & impl();

        boost::scoped_ptr<Impl> impl_;

        friend struct SetIncremental;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Parse.cci"
//#include "Parse.ct"
//#include "Parse.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
