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
        
        \li the type of command: builtin or normal command represented by a possibly relative path
            into the command tree.
        \li the command
        \li the arguments. Every argument consists of a range of ArgumentToken instances.
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
                                             command, otherwise the id of the builtin command */
        CommandPathRange commandPath() const; ///< Command path
                                        /**< This is the path to the command if it is not a builtin
                                             command. Every element of the returned range
                                             constitutes one path element. If the first element is
                                             empty, the path is an absolute path, otherwise it is
                                             relative. If the last element is an empty string, the
                                             path ends in a '/' char. */
        ArgumentsRange arguments() const; ///< Command arguments
                                        /**< The returned range contains one token range for each
                                             agument. */
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
