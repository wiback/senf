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

///////////////////////////////////////////////////////////////////////////
// senf::console::Executor

prefix_ bool senf::console::Executor::operator()(ParseCommandInfo const & command,
                                                 std::ostream & output)
{
    SENF_LOG(( "Executing: " << command ));

    if (cwd_.expired())
        cwd_ = boost::static_pointer_cast<DirectoryNode>(
            root().shared_from_this());

    switch(command.builtin()) {
    case ParseCommandInfo::NoBuiltin :
        break;

    case ParseCommandInfo::BuiltinCD :
        if ( command.arguments() &&
             ! chdir(command.arguments().begin()[0]) )
            output << "invalid directory\n";
        break;

    case ParseCommandInfo::BuiltinLS :
        for (DirectoryNode::child_iterator i (cwd().children().begin());
             i != cwd().children().end(); ++i) {
            output << i->first;
            try {
                (void) cwd()(i->first);
            }
            catch (std::bad_cast &) {
                output << "/";
            }
            output << "\n";
        }
        break;

    case ParseCommandInfo::BuiltinPUSHD :
        dirstack_.push_back(cwd_);
        if ( command.arguments()
             && ! chdir(command.arguments().begin()[0]) )
            output << "invalid directory\n";
        break;

    case ParseCommandInfo::BuiltinPOPD :
        if (! dirstack_.empty()) {
            cwd_ = dirstack_.back();
            dirstack_.pop_back();
        }
        break;

    case ParseCommandInfo::BuiltinEXIT :
        throw ExitException();
    }
    return true;
}

prefix_ bool senf::console::Executor::chdir(ParseCommandInfo::argument_value_type const & path)
{
    if (path.size() == 1 && path.begin()->value() == "-") {
        if (oldCwd_.expired()) {
            oldCwd_ = cwd_;
            cwd_ = boost::static_pointer_cast<DirectoryNode>(
                root().shared_from_this());
        } else
            swap(cwd_, oldCwd_);
    }
    else {
        try {
            DirectoryNode::ptr dir (cwd_.lock());
            ParseCommandInfo::token_iterator i (path.begin());
            ParseCommandInfo::token_iterator const i_end (path.end());
            if (i != i_end && i->value().empty()) {
                dir = boost::static_pointer_cast<DirectoryNode>(
                    root().shared_from_this());
                ++ i;
            }
            for (; i != i_end; ++i) {
                if (i->value() == "..") {
                    dir = dir->parent(); 
                    if (! dir)
                        dir = boost::static_pointer_cast<DirectoryNode>(
                            root().shared_from_this());
                }
                else if (! i->value().empty() && i->value() != ".")
                    dir = boost::static_pointer_cast<DirectoryNode>(
                        (*dir)[i->value()].shared_from_this());
            }
            oldCwd_ = cwd_;
            cwd_ = dir;
        }
        catch (std::bad_cast &) {
            return false;
        }
        catch (UnknownNodeNameException &) {
            return false;
        }
    }
    return true;
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
