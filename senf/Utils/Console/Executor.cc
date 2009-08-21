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
    \brief Executor non-inline non-template implementation */

#include "Executor.hh"
//#include "Executor.ih"

// Custom includes
#include <boost/utility.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <senf/Utils/senfassert.hh>
#include <senf/Utils/Range.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/range.hh>
#include "Server.hh"

//#include "Executor.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct TraversTokens {
        typedef std::string const & result_type;
        result_type operator()(senf::console::Token const & token) const {
            return token.value();
        }
    };
    
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Executor

prefix_ senf::console::DirectoryNode & senf::console::Executor::cwd()
    const
{
    SENF_ASSERT( ! cwd_.empty() );
    while (cwd_.size()>1 && (cwd_.back().expired() || ! cwd_.back().lock()->active()))
        cwd_.pop_back();
    return * cwd_.back().lock();
}

prefix_ std::string senf::console::Executor::cwdPath()
    const
{
    if (skipping())
        return "";
    (void) cwd(); // ensure, cwd is live.
    return "/" + senf::stringJoin(
        senf::make_transform_range(
            boost::make_iterator_range(boost::next(cwd_.begin()), cwd_.end()),
            boost::bind(&DirectoryNode::name, boost::bind(&DirectoryNode::weak_ptr::lock, _1))),
        "/" );
}

prefix_ void senf::console::Executor::execute(std::ostream & output,
                                              ParseCommandInfo const & command)
{
    SENF_LOG(( "Executing: " << command ));

    if (! skipping())
        (void) cwd(); // Prune the cwd path of expired entries

    try {
        switch(command.builtin()) {
        case ParseCommandInfo::NoBuiltin : 
            if (skipping())
                return;
            exec(output, command);
            break;

        case ParseCommandInfo::BuiltinCD :
            if (skipping())
                break;
            try {
                // The parser ensures, we have exactly one argument
                cd(command.commandPath());
            }
            catch (IgnoreCommandException &) {
                throw SyntaxErrorException(
                    "'cd' cannot be skipped (don't use 'cd' in conf-files)");
            }
            break;
            
        case ParseCommandInfo::BuiltinLS :
            if (skipping())
                break;
            // The parser ensures, we have either one or no argument
            ls( output, command.commandPath() );
            break;

        case ParseCommandInfo::BuiltinLL :
            if (skipping())
                break;
            // The parser ensures, we have either one or no argument
            ll( output, command.commandPath() );
            break;

        case ParseCommandInfo::BuiltinLR :
            if (skipping())
                break;
            // The parser ensures, we have either one or no argument
            lr( output, command.commandPath() );
            break;

        case ParseCommandInfo::BuiltinPUSHD :
            // The parser ensures, we have exactly one argument
            if (skipping())
                pushd(command.commandPath());
            else
                exec(output, command);
            break;
            
        case ParseCommandInfo::BuiltinPOPD :
            // The parser ensures, we have no arguments
            popd();
            break;
            
        case ParseCommandInfo::BuiltinEXIT :
            if (skipping())
                break;
            // The parser ensures, we have no arguments
            exit();
            break;

        case ParseCommandInfo::BuiltinHELP :
            if (skipping())
                break;
            // The parser ensures, we have either one or no arguments
            help( output, command.commandPath() );
            break;

        }
    }
    catch (InvalidPathException & ex) {
        throw SyntaxErrorException("invalid path") << " '" << ex.path << "'";
    }
    catch (InvalidDirectoryException & ex) {
        throw SyntaxErrorException("invalid directory") << " '" << ex.path << "'";
    }
    catch (InvalidCommandException &) {
        throw SyntaxErrorException("invalid command");
    }
    catch (IgnoreCommandException &) {}
}

prefix_ senf::console::GenericNode &
senf::console::Executor::getNode(ParseCommandInfo const & command)
{
    return traverseNode(command.commandPath());
}

prefix_ void senf::console::Executor::exec(std::ostream & output,
                                           ParseCommandInfo const & command)
{
    try {
        GenericNode & node ( traverseNode(command.commandPath()) );
        DirectoryNode * dir ( dynamic_cast<DirectoryNode*>(&node) );
        if ( dir ) {
            if (! command.tokens().empty())
                throw InvalidCommandException();
            if (command.builtin() == ParseCommandInfo::BuiltinPUSHD)
                pushd( command.commandPath() );
            else if (autocd_) {
                cd(command.commandPath());
            }
            else
                throw InvalidCommandException();
        } else {
            boost::any rv;
            dynamic_cast<CommandNode &>(node)(rv, output, command);
            if (command.builtin() == ParseCommandInfo::BuiltinPUSHD) {
                DirectoryNode::ptr rvdir;
                try {
                    rvdir = boost::any_cast<DirectoryNode::ptr>(rv);
                }
                catch (boost::bad_any_cast &) {
                    throw InvalidCommandException();
                }
                Path newDir (cwd_);
                newDir.push_back(rvdir);
                dirstack_.push_back(Path());
                dirstack_.back().swap(cwd_);
                cwd_.swap(newDir);
            }
        }
    }
    catch (IgnoreCommandException &) {
        if (command.builtin() == ParseCommandInfo::BuiltinPUSHD) {
            dirstack_.push_back(Path());
            dirstack_.back().swap(cwd_);
        }
        else
            throw;
    }
}


prefix_ void senf::console::Executor::cd(ParseCommandInfo::TokensRange dir)
{
    if (dir.size() == 1 && *dir.begin() == WordToken("-")) {
        cwd_.swap(oldCwd_);
        (void) cwd(); // Prune any expired items
    }
    else {
        // We need to use a temporary so an error somewhere while traversing the dir does not cause
        // the current directory to change.
        Path newDir (cwd_);
        traverseDirectory(dir, newDir);
        oldCwd_.swap(cwd_);
        cwd_.swap(newDir);
    }
}

prefix_ void senf::console::Executor::ls(std::ostream & output,
                                         ParseCommandInfo::TokensRange path)
{
    Path dir (cwd_);
    traverseDirectory(path, dir);
    DirectoryNode & node (*dir.back().lock());
    DirectoryNode::child_iterator i (node.children().begin());
    DirectoryNode::child_iterator const i_end (node.children().end());
    for (; i != i_end; ++i)
        output << i->first << "\n";
}

prefix_ void senf::console::Executor::ll(std::ostream & output,
                                         ParseCommandInfo::TokensRange path)
{
#   define HELP_COLUMN 28

    unsigned width (senf::console::Client::getWidth(output, 80u, 60u)-(HELP_COLUMN+1));
    Path dir (cwd_);
    traverseDirectory(path, dir);
    DirectoryNode & node (*dir.back().lock());
    DirectoryNode::child_iterator i (node.children().begin());
    DirectoryNode::child_iterator const i_end (node.children().end());
    boost::format fmt ("%s%s  %|" BOOST_PP_STRINGIZE(HELP_COLUMN) "t|%s\n");
    for (; i != i_end; ++i)
        output << fmt
            % i->first
            % ( i->second->isDirectory()
                ? "/"
                : i->second->isLink()
                ? "@"
                : "" )
            % i->second->shorthelp().substr(0,width);

#   undef HELP_COLUMN
}

#   define HELP_COLUMN 40

namespace {

    typedef std::map<senf::console::DirectoryNode*,std::string> NodesMap;

    void dolr(std::ostream & output, unsigned width, NodesMap & nodes, std::string const & base, 
              unsigned level, senf::console::DirectoryNode & node)
    {
        boost::format fmt ("%s%s%s  %|" BOOST_PP_STRINGIZE(HELP_COLUMN) "t|%s\n");
        std::string pad (2*level, ' ');
        senf::console::DirectoryNode::child_iterator i (node.children().begin());
        senf::console::DirectoryNode::child_iterator const i_end (node.children().end());
        for (; i != i_end; ++i) {
            if (i->second->followLink().isDirectory()) {
                senf::console::DirectoryNode & subnode (
                    static_cast<senf::console::DirectoryNode&>(i->second->followLink()));
                NodesMap::iterator j (nodes.find(&subnode));
                if (j == nodes.end()) {
                    output << fmt
                        % pad % i->first 
                        % ( i->second->isDirectory() ? "/" : i->second->isLink() ? "@" : "" )
                        % i->second->shorthelp().substr(0,width);
                    std::string subbase (base);
                    if (! subbase.empty())
                        subbase += "/";
                    subbase += i->first;
                    nodes.insert(std::make_pair(&subnode, subbase));
                    dolr(output, width, nodes, subbase, level+1, subnode);
                } else
                    output << pad << i->first 
                           << ( i->second->isDirectory() ? "/" : i->second->isLink() ? "@" : "" )
                           << " -> " << j->second << "\n";
            } else {
                output << fmt
                    % pad % i->first 
                    % ( i->second->isDirectory() ? "/" : i->second->isLink() ? "@" : "" )
                    % i->second->shorthelp().substr(0,width);
            }
        }
    }

}

prefix_ void senf::console::Executor::lr(std::ostream & output,
                                         ParseCommandInfo::TokensRange path)
{
    Path dir (cwd_);
    traverseDirectory(path, dir);
    DirectoryNode & node (*dir.back().lock());
    NodesMap nodes;
    dolr(output, senf::console::Client::getWidth(output, 80u, 60u)-(HELP_COLUMN+1), 
         nodes, "", 0, node);
}

#undef HELP_COLUMN

prefix_ void senf::console::Executor::pushd(ParseCommandInfo::TokensRange dir)
{
    Path newDir (cwd_);
    if (! skipping()) {
        try {
            traverseDirectory(dir, newDir);
        }
        catch (IgnoreCommandException &) {
            newDir.clear();
        }
    }
    dirstack_.push_back(Path());
    dirstack_.back().swap(cwd_);
    cwd_.swap(newDir);
}

prefix_ void senf::console::Executor::popd()
{
    if (! dirstack_.empty()) {
        cwd_.swap(dirstack_.back());
        dirstack_.pop_back();
    }
}

prefix_ void senf::console::Executor::exit()
{
    throw ExitException();
}

prefix_ void senf::console::Executor::help(std::ostream & output,
                                           ParseCommandInfo::TokensRange path)
{
    GenericNode const & node (traverseNode(path));
    // output << prettyName(typeid(node)) << " at " << node.path() << "\n\n";
    node.help(output);
    output << std::flush;
}

prefix_ senf::console::GenericNode &
senf::console::Executor::traverseNode(ParseCommandInfo::TokensRange const & path)
{
    if (path.empty())
        return *cwd_.back().lock();
    try {
        Path dir (cwd_);
        traverseDirectory(boost::make_iterator_range(
                              path.begin(),
                              boost::prior(path.end())),
                          dir);
        // For auto-cd support we need to check against '.' and '..' here too
        Token const & tok (*boost::prior(path.end()));
        if (tok == WordToken("..")) {
            if (dir.size() > 1)
                dir.pop_back();
            return *dir.back().lock();
        }
        DirectoryNode & base (*dir.back().lock());
        if (tok == WordToken(".") || tok == NoneToken())
            return base;
        std::string const & name (complete(base, tok.value()));
        if (policy_)
            policy_( base, name );
        return dir.back().lock()->get(name);
    }
    catch (UnknownNodeNameException &) {
        throw InvalidPathException(
            senf::stringJoin(
                senf::make_transform_range(path, boost::bind(&Token::value, _1)),
                "/"));
    }
}

prefix_ void
senf::console::Executor::traverseDirectory(ParseCommandInfo::TokensRange const & path,
                                           Path & dir)
{
    std::string errorPath;
    try {
        ParseCommandInfo::TokensRange::const_iterator i (path.begin());
        ParseCommandInfo::TokensRange::const_iterator const i_end (path.end());
        for (; i != i_end; ++i) {
            if (i != path.begin())
                errorPath += "/";
            errorPath += i->value();
            if (*i == NoneToken()) {
                if (i == path.begin()) {
                    dir.clear();
                    dir.push_back(root_);
                }
            }
            else if (*i == WordToken("..")) {
                if (dir.size() > 1)
                    dir.pop_back();
            }
            else if (*i ==  WordToken("."))
                ;
            else {
                DirectoryNode & base (*dir.back().lock());
                std::string name (complete(base, i->value()));
                if (policy_)
                    policy_( base, name );
                dir.push_back(base[name].thisptr());
            }
        }
    }
    catch (std::bad_cast &) {
        throw InvalidDirectoryException(errorPath);
    }
    catch (UnknownNodeNameException &) {
        throw InvalidDirectoryException(errorPath);
    }
}

prefix_ std::string senf::console::Executor::complete(DirectoryNode & dir,
                                                      std::string const & name)
{
    if (! dir.hasChild(name)) {
        DirectoryNode::ChildrenRange completions (dir.completions(name));
        if (has_one_elt(completions))
            return completions.begin()->first;
    }
    return name;
}

prefix_ void senf::console::senf_console_format_value(DirectoryNode::ptr value,
                                                      std::ostream & os)
{
    if (value)
        os << "<Directory at '" << value->path() << "'>";
    else
        os << "<Null Directory>";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Executor.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
