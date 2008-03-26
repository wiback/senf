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

//#include "Executor.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct TraverseTokens {
        typedef std::string const & result_type;
        result_type operator()(senf::console::ArgumentToken const & token) const {
            return token.value();
        }
    };
    
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Executor

prefix_ bool senf::console::Executor::operator()(ParseCommandInfo const & command,
                                                 std::ostream & output)
{
    SENF_LOG(( "Executing: " << command ));

    if (cwd_.expired() || ! cwd().active())
        cwd_ = root().thisptr();

    try {
        switch(command.builtin()) {
        case ParseCommandInfo::NoBuiltin :
            traverseToCommand(command.commandPath())(output, command.arguments());
            break;

        case ParseCommandInfo::BuiltinCD :
            if ( command.arguments() ) {
                if (command.arguments().begin()->size() == 1 
                    && command.arguments().begin()->begin()->value() == "-") {
                    if (oldCwd_.expired() || ! oldCwd_.lock()->active()) {
                        oldCwd_ = cwd_;
                        cwd_ = root().thisptr();
                    } else
                        swap(cwd_, oldCwd_);
                }
                else {
                    oldCwd_ = cwd_;
                    cwd_ = traverseTo(command.arguments().begin()[0]).thisptr();
                }
            }
            break;
            
        case ParseCommandInfo::BuiltinLS : {
            DirectoryNode const & dir (
                command.arguments().empty() ? cwd() : traverseTo(command.arguments().begin()[0]));
            for (DirectoryNode::child_iterator i (dir.children().begin());
                 i != dir.children().end(); ++i) {
                output << i->first;
                if (boost::dynamic_pointer_cast<DirectoryNode>(i->second))
                    output << "/";
                output << "\n";
            }
            break;
        }

        case ParseCommandInfo::BuiltinPUSHD :
            dirstack_.push_back(cwd_);
            if ( command.arguments() )
                cwd_ = traverseTo(command.arguments().begin()[0]).thisptr();
            break;
            
        case ParseCommandInfo::BuiltinPOPD :
            if (! dirstack_.empty()) {
                cwd_ = dirstack_.back();
                dirstack_.pop_back();
            }
            break;
            
        case ParseCommandInfo::BuiltinEXIT :
            throw ExitException();

        case ParseCommandInfo::BuiltinHELP :
            try {
                GenericNode & node (
                    command.arguments() 
                    ? cwd().traverse(
                        boost::make_iterator_range(
                            boost::make_transform_iterator(command.arguments().begin()[0].begin(), TraverseTokens()),
                            boost::make_transform_iterator(command.arguments().begin()[0].end(), TraverseTokens())))
                    : cwd() );
                node.help(output);
                output << std::flush;
            }
            catch (UnknownNodeNameException &) {
                output << "invalid path" << std::endl;
            }
            break;
        }
    }
    catch (InvalidDirectoryException &) {
        output << "invalid directory" << std::endl;
    }
    catch (InvalidCommandException &) {
        output << "invalid command" << std::endl;
    }
    return true;
}

prefix_ senf::console::DirectoryNode &
senf::console::Executor::traverseTo (ParseCommandInfo::argument_value_type const & path)
{
    try {
        return dynamic_cast<DirectoryNode&>(
            cwd().traverse(
                boost::make_iterator_range(
                    boost::make_transform_iterator(path.begin(), TraverseTokens()),
                    boost::make_transform_iterator(path.end(), TraverseTokens()))));
    }
    catch (std::bad_cast &) {
        throw InvalidDirectoryException();
    }
    catch (UnknownNodeNameException &) {
        throw InvalidDirectoryException();
    }
}

prefix_ senf::console::CommandNode &
senf::console::Executor::traverseToCommand(ParseCommandInfo::CommandPathRange const & path)
{
    try {
        return dynamic_cast<CommandNode &>( cwd().traverse(path) );
    }
    catch (std::bad_cast &) {
        throw InvalidCommandException();
    }
    catch (UnknownNodeNameException &) {
        throw InvalidCommandException();
    }        
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
