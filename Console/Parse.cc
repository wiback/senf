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
    \brief Parse non-inline non-template implementation */

#include "Parse.hh"
#include "Parse.ih"

// Custom includes
#include <cerrno>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include "../Utils/String.hh"
#include "../Utils/Exception.hh"

//#include "Parse.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

#ifndef DOXYGEN

    struct ParserAccess
    {
        static void init(ParseCommandInfo & info)
            { info.init(); }

        static void setBuiltin(ParseCommandInfo & info, ParseCommandInfo::BuiltinCommand builtin)
            { info.setBuiltin(builtin); }

        static void setCommand(ParseCommandInfo & info, std::vector<std::string> & commandPath)
            { info.setCommand(commandPath); }

        static void addToken(ParseCommandInfo & info, ArgumentToken const & token)
            { info.addToken(token); }

        static ArgumentToken makeToken(ArgumentToken::TokenType type, std::string const & token)
            { return ArgumentToken(type, token); }
    };

    struct ParseDispatcher
    {
        ParseCommandInfo info_;
        CommandParser::Callback cb_;

        struct BindInfo {
            BindInfo( ParseDispatcher & d, CommandParser::Callback cb)
                : dispatcher (d) { dispatcher.cb_ = cb; }
            ~BindInfo() { dispatcher.cb_  = 0; }

            ParseDispatcher & dispatcher;
        };

        void beginCommand(std::vector<std::string> & command)
            { ParserAccess::init(info_);
              ParserAccess::setCommand(info_, command); }

        void endCommand()
            { cb_(info_); }

        void pushArgument(ArgumentToken::TokenType type, std::string const & argument)
            { ParserAccess::addToken(info_, ParserAccess::makeToken(type, argument)); }

        void openGroup()
            { pushPunctuation("("); }

        void closeGroup()
            { pushPunctuation(")"); }

        void pushPunctuation(std::string const & token)
            {
                ArgumentToken::TokenType type;
                switch (token[0]) {
                case '/' : type = ArgumentToken::PathSeparator; break;
                case '(' : type = ArgumentToken::ArgumentGroupOpen; break;
                case ')' : type = ArgumentToken::ArgumentGroupClose; break;
                case '{' : type = ArgumentToken::DirectoryGroupOpen; break;
                case '}' : type = ArgumentToken::DirectoryGroupClose; break;
                case ';' : type = ArgumentToken::CommandTerminator; break;
                default :  type = ArgumentToken::OtherPunctuation; break;
                }
                ParserAccess::addToken(info_, ParserAccess::makeToken(type, token));
            }

        void pushWord(ArgumentToken::TokenType type, std::string const & token)
            { ParserAccess::addToken(info_, ParserAccess::makeToken(type, token)); }

        void builtin_cd(std::vector<std::string> & path)
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinCD);
              setBuiltinPathArg(path);
              cb_(info_); }

        void builtin_ls(std::vector<std::string> & path)
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinLS);
              setBuiltinPathArg(path);
              cb_(info_); }

        void pushDirectory(std::vector<std::string> & path)
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinPUSHD);
              setBuiltinPathArg(path);
              cb_(info_); }

        void popDirectory()
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinPOPD);
              cb_(info_); }
        
        void builtin_exit()
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinEXIT);
              cb_(info_); }

        void builtin_help(std::vector<std::string> & path)
            { ParserAccess::init(info_);
              ParserAccess::setBuiltin(info_, ParseCommandInfo::BuiltinHELP);
              setBuiltinPathArg(path);
              cb_(info_); }

        void setBuiltinPathArg(std::vector<std::string> & path)
            {
                pushPunctuation("(");
                for (std::vector<std::string>::const_iterator i (path.begin());
                     i != path.end(); ++i)
                    ParserAccess::addToken(info_, 
                                           ParserAccess::makeToken(ArgumentToken::Word, *i));
                pushPunctuation(")");
            }
    };

#endif

}}}

///////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo

prefix_ std::ostream & senf::console::operator<<(std::ostream & stream,
                                                 ParseCommandInfo const & info)
{
    if (info.builtin() == ParseCommandInfo::NoBuiltin) 
        stream << senf::stringJoin(info.commandPath(), "/");
    else {
        char const * builtins[] = { "", "cd", "ls", "pushd", "popd", "exit", "help" };
        stream << "builtin-" << builtins[info.builtin()];
    }
        
    ParseCommandInfo::ArgumentsRange args (info.arguments());
    for (ParseCommandInfo::argument_iterator i (args.begin()); i != args.end(); ++i) {
        ParseCommandInfo::token_iterator j (i->begin());
        stream << " [";
        if ( j != i->end() ) {
            for (;;) {
                stream << "'" << j->value() << "'";
                if ( ++j != i->end() ) stream << ' ';
                else                   break;
            }
        }
        stream << "]";
    }

    return stream;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo::ArgumentIterator

prefix_ void senf::console::ParseCommandInfo::ArgumentIterator::setRange()
    const
{
    if (b_->is(ArgumentToken::ArgumentGroupOpen)) {
        unsigned level (0);
        e_ = b_;
        for (;;) {
            if (e_->is(ArgumentToken::ArgumentGroupOpen))
                ++ level;
            else if (e_->is(ArgumentToken::ArgumentGroupClose)) {
                -- level;
                if (level == 0)
                    break;
            }
            ++e_;
        }
    }
    ++ e_;
}

prefix_ void senf::console::ParseCommandInfo::ArgumentIterator::decrement()
{
    e_ = b_;
    --b_;
    if (b_->is(ArgumentToken::ArgumentGroupClose)) {
        unsigned level (0);
        for (;;) {
            if (b_->is(ArgumentToken::ArgumentGroupClose))
                ++ level;
            else if (b_->is(ArgumentToken::ArgumentGroupOpen)) {
                -- level;
                if (level == 0)
                    break;
            }
            --b_;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::console::CommandParser

#ifndef DOXYGEN

struct senf::console::CommandParser::Impl
{
    typedef detail::CommandGrammar<detail::ParseDispatcher> Grammar;

    detail::ParseDispatcher dispatcher;
    Grammar::Context context;
    Grammar grammar;

    Impl() : dispatcher(), context(), grammar(dispatcher, context) {}
};

#endif

prefix_ senf::console::CommandParser::CommandParser()
    : impl_ (new Impl())
{}

prefix_ senf::console::CommandParser::~CommandParser()
{}

prefix_ bool senf::console::CommandParser::parse(std::string command, Callback cb)
{
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, cb);
    return boost::spirit::parse( command.begin(), command.end(), 
                                 impl().grammar.use_parser<Impl::Grammar::CommandParser>(),
                                 impl().grammar.use_parser<Impl::Grammar::SkipParser>()
        ).full;
}

prefix_ bool senf::console::CommandParser::parseFile(std::string filename, Callback cb)
{
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, cb);
    boost::spirit::file_iterator<> i (filename);
    if (!i) throw SystemException(ENOENT SENF_EXC_DEBUGINFO);
    boost::spirit::file_iterator<> const i_end (i.make_end());
    
    return boost::spirit::parse( i, i_end, 
                                 impl().grammar.use_parser<Impl::Grammar::CommandParser>(),
                                 impl().grammar.use_parser<Impl::Grammar::SkipParser>()
        ).full;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Parse.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
