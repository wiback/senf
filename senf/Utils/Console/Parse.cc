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
    \brief Parse non-inline non-template implementation */

#include "Parse.hh"
#include "Parse.ih"

// Custom includes
#include <cerrno>
#include <sys/stat.h>
#include <boost/iterator/transform_iterator.hpp>
#include <senf/config.hh>

#include <boost/spirit/include/classic_file_iterator.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

#include <senf/Utils/Exception.hh>
#include <senf/Utils/senfassert.hh>

//#include "Parse.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

#ifndef DOXYGEN

    struct ParseDispatcher
    {
        ParseCommandInfo * info_;

        struct BindInfo {
            BindInfo( ParseDispatcher & d, ParseCommandInfo & info)
                : dispatcher (d) { dispatcher.info_ = &info; }
            ~BindInfo() { dispatcher.info_ = 0; }

            ParseDispatcher & dispatcher;
        };

        void beginCommand(std::vector<Token> & command)
            { info_->clear();
              info_->command(command); }

        void endCommand()
            { }

        void pushToken(Token const & token)
            { info_->addToken(token); }

        void builtin_cd(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinCD);
              setBuiltinPathArg(path); }

        void builtin_ls(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinLS);
              setBuiltinPathArg(path); }

        void builtin_ll(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinLL);
              setBuiltinPathArg(path); }

        void builtin_lr(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinLR);
              setBuiltinPathArg(path); }

        void pushDirectory()
            { // Do NOT call clear since pushDirectory is set in ADDITION
              // to an ordinary command (which may be only a directory name)
              info_->builtin(ParseCommandInfo::BuiltinPUSHD); }

        void popDirectory()
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinPOPD); }

        void builtin_exit()
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinEXIT); }

        void builtin_help(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinHELP);
              setBuiltinPathArg(path); }

        void builtin_echo()
            { info_->builtin(ParseCommandInfo::BuiltinECHO); }

        void setBuiltinPathArg(std::vector<Token> & path)
            {
                info_->command(path);
//                 pushToken(ArgumentGroupOpenToken());
//                 for (std::vector<Token>::const_iterator i (path.begin());
//                      i != path.end(); ++i)
//                     pushToken(*i);
//                 pushToken(ArgumentGroupCloseToken());
            }
    };

#endif

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::Token

prefix_ senf::console::Token::Token(TokenType type, std::string token,
                                    detail::FilePositionWithIndex const & pos)
    : type_(type), token_ (token), line_ (pos.line), column_ (pos.column), index_ (pos.index)
{}


prefix_ std::ostream & senf::console::operator<<(std::ostream & os, Token const & token)
{
    static char const * tokenTypeName[] = {
        "None",
        "PathSeparator",
        "ArgumentGroupOpen",
        "ArgumentGroupClose",
        "DirectoryGroupOpen",
        "DirectoryGroupClose",
        "CommandTerminator",
        "OtherPunctuation",
        "BasicString",
        "HexString",
        "Word" };
    // The real table is:
    //     static const int bitPosition[32] = {
    //          0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
    //         31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9 };
    // However, we have replaced all values >= sizeof(tokenTypeName) with 0
    // and have added 1 to all the remaining values
    static const int bitPosition[32] = {
        1, 2, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 5, 9,
        0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 7, 0, 6, 0, 10 };
    // We need to check token.type() against 0 explicitly since 0 and 1 will both be mapped to 0
    os << tokenTypeName[ token.type()
                         ? bitPosition[(((token.type() & -token.type()) * 0x077CB531UL) >> 27) & 31]
                         : 0 ]
       << "('"
       << token.value()
       << "')";
    return os;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo

prefix_ std::ostream & senf::console::operator<<(std::ostream & stream,
                                                 ParseCommandInfo const & info)
{
    if (info.builtin() == ParseCommandInfo::NoBuiltin) {
        ParseCommandInfo::TokensRange::const_iterator i (info.commandPath().begin());
        ParseCommandInfo::TokensRange::const_iterator const i_end (info.commandPath().end());
        if (i != i_end) {
            for (;;) {
                stream << i->value();
                if ( ++i != i_end ) stream << "/";
                else                break;
            }
        }
    }
    else {
        char const * builtins[] = { 0, "cd", "ls", "lr", "pushd", "popd", "exit", "help" };
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo::ArgumentIterator

prefix_ void senf::console::ParseCommandInfo::ArgumentIterator::setRange()
    const
{
    if (b_->is(Token::ArgumentGroupOpen)) {
        unsigned level (0);
        e_ = b_;
        for (;;) {
            if (e_->is(Token::ArgumentGroupOpen))
                ++ level;
            else if (e_->is(Token::ArgumentGroupClose)) {
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
    if (b_->is(Token::ArgumentGroupClose)) {
        unsigned level (0);
        for (;;) {
            if (b_->is(Token::ArgumentGroupClose))
                ++ level;
            else if (b_->is(Token::ArgumentGroupOpen)) {
                -- level;
                if (level == 0)
                    break;
            }
            --b_;
        }
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

namespace {

    template <class Error>
    void throwParserError(Error const & err)
    {
        static char const * msg [] = { "end of statement expected",
                                       "path expected",
                                       "')' expected",
                                       "'\"' expected" };
        senf::console::detail::FilePositionWithIndex pos (err.where.get_position());
        throw senf::console::CommandParser::ParserErrorException(msg[err.descriptor])
            << "\nat " << pos.file << ":" << pos.line << ":" << pos.column;
    }

}

namespace boost {
namespace spirit {
namespace classic {

    template <>
    class position_policy<senf::console::detail::FilePositionWithIndex>
        : public position_policy<file_position>
    {
    public:
        typedef position_policy<file_position> Base;

        void next_line(senf::console::detail::FilePositionWithIndex & pos)
            {
                Base::next_line(pos);
                pos.index ++;
            }

        void next_char(senf::console::detail::FilePositionWithIndex & pos)
            {
                Base::next_char(pos);
                pos.index ++;
            }

        void tabulation(senf::console::detail::FilePositionWithIndex & pos)
            {
                Base::tabulation(pos);
                pos.index ++;
            }
    };

}}}

prefix_ senf::console::CommandParser::CommandParser()
    : impl_ (new Impl())
{}

prefix_ senf::console::CommandParser::~CommandParser()
{}

// This template member is placed here, since it is ONLY called from the implementation.  Otherwise,
// we would need to expose the Impl member to the public, which we don't want to do.

template <class Iterator>
prefix_ Iterator senf::console::CommandParser::parseLoop(Iterator npb, Iterator npe,
                                                         std::string const & source, Callback cb)
{
    typedef detail::boost_spirit::position_iterator<
        Iterator, detail::FilePositionWithIndex> PositionIterator;
    PositionIterator b (npb, npe, source);
    PositionIterator e (npe, npe, source);
    ParseCommandInfo info;
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    detail::boost_spirit::parse_info<PositionIterator> result;

    for (;;) {
        result = detail::boost_spirit::parse(
            b, e, * impl().grammar.use_parser<Impl::Grammar::SkipParser>());
        b = result.stop;
        if (b == e)
            return e.base();
        info.clear();
        try {
            result = detail::boost_spirit::parse(b, e,
                                          impl().grammar.use_parser<Impl::Grammar::CommandParser>(),
                                          impl().grammar.use_parser<Impl::Grammar::SkipParser>());
        }
        catch (detail::boost_spirit::parser_error<Impl::Grammar::Errors, PositionIterator> & ex) {
            if (impl().grammar.incremental && ex.where == e)
                return b.base();
            else
                throwParserError(ex);
        }
        // Otherwise the error handling in the parser is broken
        SENF_ASSERT( result.hit, "Internal parser failure (error handling broken?)" );
        if (! info.empty())
            try {
                cb(info);
            }
            catch (senf::ExceptionMixin & ex) {
                detail::FilePositionWithIndex pos (result.stop.get_position());
                ex << "\nat " << pos.file << ":" << pos.line << ":" << pos.column;
                throw;
            }
        b = result.stop;
    }
}

prefix_ void senf::console::CommandParser::parse(std::string const & command, Callback cb)
{
    parseLoop(command.begin(), command.end(), "<unknown>", cb);
}

prefix_ void senf::console::CommandParser::parseFile(std::string const & filename, Callback cb)
{
    // file_iterator sets errno to EINVAL and returns error when file size is 0
    // so we check the file size before
    struct stat statBuf;
    if (stat( filename.c_str(), &statBuf) != 0)
        throw SystemException(filename, errno SENF_EXC_DEBUGINFO);
    if (statBuf.st_size == 0) return;
    detail::boost_spirit::file_iterator<> i (filename);
    if (!i) {
        if (errno == 0)
            // hmm.. errno==0 but the file_iterator is false; something is wrong but we
            // do not know what exactly, so we throw a SystemeException with EINVAL
            throw SystemException(filename, EINVAL SENF_EXC_DEBUGINFO);
        else
            throw SystemException(filename, errno SENF_EXC_DEBUGINFO);
    }
    detail::boost_spirit::file_iterator<> const i_end (i.make_end());
    parseLoop(i, i_end, filename, cb);
}

prefix_ void senf::console::CommandParser::parseArguments(std::string const & arguments,
                                                          ParseCommandInfo & info)
{
    typedef detail::boost_spirit::position_iterator<
        std::string::const_iterator, detail::FilePositionWithIndex> PositionIterator;
    PositionIterator b (arguments.begin(), arguments.end(), std::string("<unknown>"));
    PositionIterator e (arguments.end(), arguments.end(), std::string("<unknown>"));
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    detail::boost_spirit::parse_info<PositionIterator> result;
    try {
        result = detail::boost_spirit::parse( b, e,
                                       impl().grammar.use_parser<Impl::Grammar::ArgumentsParser>(),
                                       impl().grammar.use_parser<Impl::Grammar::SkipParser>() );
    }
    catch (detail::boost_spirit::parser_error<Impl::Grammar::Errors, PositionIterator> & ex) {
        throwParserError(ex);
    }
    if (! result.full) {
        detail::FilePositionWithIndex pos (result.stop.get_position());
        throw ParserErrorException("argument expected")
            << "\nat " << pos.file << ":" << pos.line << ":" << pos.column;
    }
}

prefix_ void senf::console::CommandParser::parsePath(std::string const & path,
                                                     ParseCommandInfo & info)
{
    typedef detail::boost_spirit::position_iterator<
        std::string::const_iterator, detail::FilePositionWithIndex> PositionIterator;
    PositionIterator b (path.begin(), path.end(), std::string("<unknown>"));
    PositionIterator e (path.end(), path.end(), std::string("<unknown>"));
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    detail::boost_spirit::parse_info<PositionIterator> result;
    try {
        result = detail::boost_spirit::parse( b, e,
                                       impl().grammar.use_parser<Impl::Grammar::PathParser>(),
                                       impl().grammar.use_parser<Impl::Grammar::SkipParser>() );
    }
    catch (detail::boost_spirit::parser_error<Impl::Grammar::Errors, PositionIterator> & ex) {
        throwParserError(ex);
    }
    if (! result.full) {
        detail::FilePositionWithIndex pos (result.stop.get_position());
        throw ParserErrorException("path expected")
            << "\nat " << pos.file << ":" << pos.line << ":" << pos.column;
    }
}

struct senf::console::CommandParser::SetIncremental
{
    SetIncremental(CommandParser & parser) : parser_ (parser) {
        parser_.impl().grammar.incremental = true;
    }

    ~SetIncremental() {
        parser_.impl().grammar.incremental = false;
    }

    CommandParser & parser_;
};

prefix_ std::string::size_type
senf::console::CommandParser::parseIncremental(std::string const & commands, Callback cb)
{
    SetIncremental si (*this);
    return std::distance( commands.begin(),
                          parseLoop(commands.begin(), commands.end(), "<unknown>", cb) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// Character sets

prefix_ bool senf::console::CommandParser::isSpecialChar(char ch)
{
    return Impl::Grammar::special_p().test(ch);
}

prefix_ bool senf::console::CommandParser::isPunctuationChar(char ch)
{
    return Impl::Grammar::punctuation_p().test(ch);
}

prefix_ bool senf::console::CommandParser::isSpaceChar(char ch)
{
    return Impl::Grammar::space_p().test(ch);
}

prefix_ bool senf::console::CommandParser::isInvalidChar(char ch)
{
    return Impl::Grammar::invalid_p().test(ch);
}

prefix_ bool senf::console::CommandParser::isWordChar(char ch)
{
    return Impl::Grammar::word_p().test(ch);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
