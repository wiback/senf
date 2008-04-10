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
    \brief Parse public header */

#ifndef HH_Parse_
#define HH_Parse_ 1

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
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/function.hpp>

//#include "Parse.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    namespace detail { struct ParserAccess; }

    /** \brief Single argument token

        All command arguments are split into tokens by the parser. Each token is returned as an
        ArgumentToken instance. 

        \ingroup console_parser
      */
    class ArgumentToken
    {
    public:
        std::string const & value() const; ///< String value of token
                                        /**< This value is properly unquoted */

    protected:

    private:
        explicit ArgumentToken(std::string token);

        std::string token_;

        friend class detail::ParserAccess;
    };

    /** \brief Console command

        Every command parsed is returned in a ParseCommandInfo instance. This information is purely
        taken from the parser, no semantic information is attached at this point, the config/console
        is not involved in any why. ParseCommandInfo consist of
        
        \li the type of command: built-in or normal command represented by a possibly relative path
            into the command tree.
        \li the command
        \li the arguments. Every argument consists of a range of ArgumentToken instances.

        \ingroup console_parser
      */
    class ParseCommandInfo
    {
        typedef std::vector<ArgumentToken> Tokens;
        typedef std::vector<std::string> CommandPath;
        
    public:
        typedef CommandPath::const_iterator path_iterator;
        typedef Tokens::const_iterator token_iterator;
        typedef boost::iterator_range<token_iterator> argument_value_type;


    private:
        typedef std::vector<argument_value_type> Arguments;

    public:
        typedef Arguments::const_iterator argument_iterator;
        typedef Arguments::size_type size_type;

        typedef boost::iterator_range<path_iterator> CommandPathRange;
        typedef boost::iterator_range<argument_iterator> ArgumentsRange;
        typedef boost::iterator_range<token_iterator> TokensRange;

        enum BuiltinCommand { NoBuiltin, 
                              BuiltinCD, 
                              BuiltinLS, 
                              BuiltinPUSHD, 
                              BuiltinPOPD,
                              BuiltinEXIT,
                              BuiltinHELP };

        BuiltinCommand builtin() const; ///< Command type
                                        /**< \returns \c NoBuiltin, if the command is an ordinary
                                             command, otherwise the id of the built-in command */
        CommandPathRange commandPath() const; ///< Command path
                                        /**< This is the path to the command if it is not a built-in
                                             command. Every element of the returned range
                                             constitutes one path element. If the first element is
                                             empty, the path is an absolute path, otherwise it is
                                             relative. If the last element is an empty string, the
                                             path ends in a '/' char. */
        ArgumentsRange arguments() const; ///< Command arguments
                                        /**< The returned range contains one token range for each
                                             argument. */
        TokensRange tokens() const;     ///< All argument tokens
                                        /**< The returned range contains \e all argument tokens in a
                                             single range not divided into separate arguments. */
    protected:

    private:
        void init();
        void setBuiltin(BuiltinCommand builtin);
        void setCommand(std::vector<std::string> & commandPath);
        void startArgument();
        void endArgument();
        void addToken(ArgumentToken const & token);
        void finalize();

        struct MakeRange;

        std::vector<std::string> commandPath_;

        typedef std::pair<Tokens::size_type, Tokens::size_type> TempArgumentRange;
        typedef std::vector<TempArgumentRange> TempArguments;

        BuiltinCommand builtin_;
        Tokens tokens_;
        Arguments arguments_;
        TempArguments tempArguments_;

        friend class detail::ParserAccess;
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

        \todo Implement more detailed error reporting and error recovery.

        \ingroup console_parser
      */
    class CommandParser
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (ParseCommandInfo const &)> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        CommandParser();
        ~CommandParser();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool parse(std::string command, Callback cb); ///< Parse string
        bool parseFile(std::string filename, Callback cb); ///< Parse file
                                        /**< \throws SystemException if the file cannot be
                                             read. */

    private:
        struct Impl;

        Impl & impl();

        boost::scoped_ptr<Impl> impl_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
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
