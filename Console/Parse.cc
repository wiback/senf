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
#include "../Utils/String.hh"
#include <boost/iterator/transform_iterator.hpp>

//#include "Parse.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

    struct ParserAccess
    {
        static void init(ParseCommandInfo & info)
            { info.init(); }

        static void setBuiltin(ParseCommandInfo & info, ParseCommandInfo::BuiltinCommand builtin)
            { info.setBuiltin(builtin); }

        static void setCommand(ParseCommandInfo & info, std::vector<std::string> & commandPath)
            { info.setCommand(commandPath); }

        static void startArgument(ParseCommandInfo & info)
            { info.startArgument(); }

        static void endArgument(ParseCommandInfo & info)
            { info.endArgument(); }

        static void addToken(ParseCommandInfo & info, ArgumentToken const & token)
            { info.addToken(token); }

        static void finalize(ParseCommandInfo & info)
            { info.finalize(); }

        static ArgumentToken makeToken(std::string const & token)
            { return ArgumentToken(token); }
    };

    struct ParseDispatcher
    {
        ParseDispatcher()
            : info_ (0) {}
        
        ParseCommandInfo * info_;

        ParseCommandInfo & info() {
            SENF_ASSERT( info_ );
            return *info_;
        }

        struct BindInfo {
            BindInfo( ParseDispatcher & d, ParseCommandInfo & i)
                : dispatcher (d) { dispatcher.info_ = &i; }

            ~BindInfo() { dispatcher.info_  = 0; }

            ParseDispatcher & dispatcher;
        };

        void beginCommand(std::vector<std::string> & command)
            { ParserAccess::init(info());
              ParserAccess::setCommand(info(), command); }

        void endCommand()
            { ParserAccess::finalize(info()); }

        void pushArgument(std::string const & argument)
            { ParserAccess::startArgument(info()); 
              ParserAccess::addToken(info(), ParserAccess::makeToken(argument)); 
              ParserAccess::endArgument(info()); }

        void openGroup()
            { ParserAccess::startArgument(info()); }

        void closeGroup()
            { ParserAccess::endArgument(info()); }

        void pushPunctuation(std::string const & token)
            { ParserAccess::addToken(info(), ParserAccess::makeToken(token)); }

        void pushWord(std::string const & token)
            { ParserAccess::addToken(info(), ParserAccess::makeToken(token)); }

        void builtin_cd(std::vector<std::string> & path)
            { ParserAccess::init(info());
              ParserAccess::setBuiltin(info(), ParseCommandInfo::BuiltinCD);
              setBuiltinPathArg(path);
              ParserAccess::finalize(info()); }

        void builtin_ls(std::vector<std::string> & path)
            { ParserAccess::init(info());
              ParserAccess::setBuiltin(info(), ParseCommandInfo::BuiltinLS);
              setBuiltinPathArg(path);
              ParserAccess::finalize(info()); }

        void setBuiltinPathArg(std::vector<std::string> & path)
            {
                ParserAccess::startArgument(info());
                for (std::vector<std::string>::const_iterator i (path.begin());
                     i != path.end(); ++i)
                    ParserAccess::addToken(info(), ParserAccess::makeToken(*i));
                ParserAccess::endArgument(info());
            }
    };

}}}

///////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo

struct senf::console::ParseCommandInfo::MakeRange
{
    MakeRange() {}
    MakeRange(ParseCommandInfo::token_iterator b) : b_ (b) {}

    senf::console::ParseCommandInfo::token_iterator b_;

    typedef ParseCommandInfo::argument_value_type result_type;
        
    result_type operator()(TempArguments::iterator::value_type const & v) const {
        return result_type( b_ + v.first, b_ + v.second );
    }
};

prefix_ void senf::console::ParseCommandInfo::finalize()
{
    arguments_.resize( tempArguments_.size() );

    std::copy( boost::make_transform_iterator( tempArguments_.begin(), 
                                               MakeRange(tokens_.begin()) ),
               boost::make_transform_iterator( tempArguments_.end(), 
                                               MakeRange() ),
               arguments_.begin() );

    tempArguments_.clear();
}

prefix_ std::ostream & senf::console::operator<<(std::ostream & stream,
                                                 ParseCommandInfo const & info)
{
    if (info.builtin() == ParseCommandInfo::NoBuiltin) 
        stream << senf::stringJoin(info.commandPath(), "/");
    else {
        char const * builtins[] = { "", "cd", "ls" };
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
// senf::console::SingleCommandParser

struct senf::console::SingleCommandParser::Impl
{
    typedef detail::CommandGrammar<detail::ParseDispatcher> Grammar;

    detail::ParseDispatcher dispatcher;
    Grammar::Context context;
    Grammar grammar;

    Impl() : dispatcher(), context(), grammar(dispatcher, context) {}
};

prefix_ senf::console::SingleCommandParser::SingleCommandParser()
    : impl_ (new Impl())
{}

prefix_ senf::console::SingleCommandParser::~SingleCommandParser()
{}

prefix_ bool senf::console::SingleCommandParser::parseCommand(std::string command,
                                                              ParseCommandInfo & info)
{
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    return boost::spirit::parse( command.c_str(), 
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
