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
    \brief Config non-inline non-template implementation */

#include "Config.hh"
//#include "Config.ih"

// Custom includes

//#include "Config.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::ConfigFile

#ifndef DOXYGEN

namespace {
    struct BindPolicy
    {
        BindPolicy(senf::console::Executor & e, senf::console::Executor::SecurityPolicy p) 
            : e_ (e) 
            { e_.policy(p); }
        
        ~BindPolicy() 
            { e_.policy(senf::console::Executor::SecurityPolicy()); }
        
        senf::console::Executor & e_;
    };
}

#endif

prefix_ void senf::console::ConfigFile::parse(DirectoryNode & restrict)
{
    DirectoryNode::ptr r (restrict.thisptr());
    BindPolicy bp ( executor_, 
                    boost::bind(&ConfigFile::policyCallback, this, r, _1, _2) );
    if (! parser_.parseFile(filename_, boost::bind<void>( boost::ref(executor_),
                                                          boost::ref(std::cerr),
                                                          _1 )) )
        throw SyntaxErrorException();
    insertParsedNode(r);
}

prefix_ bool senf::console::ConfigFile::parsed(GenericNode & node)
    const
{
    ParsedNodes::const_iterator i (parsedNodes_.begin());
    ParsedNodes::const_iterator const i_end (parsedNodes_.end());
    for (; i != i_end; ++i)
        if ( ! i->expired() && node.isChildOf(*(i->lock())) )
            return true;
    return false;
}

prefix_ void senf::console::ConfigFile::policyCallback(DirectoryNode::ptr restrict,
                                                       DirectoryNode & dir,
                                                       std::string const & name)
{
    if (dir.hasChild(name)) {
        GenericNode & item (dir.get(name));
        if (restrict && ! item.isChildOf(*restrict)) {
            DirectoryNode * itemdir (dynamic_cast<DirectoryNode*>(&item));
            if (! itemdir || ! restrict->isChildOf(*itemdir))
                throw Executor::IgnoreCommandException();
        }
        if (parsed(item))
            throw Executor::IgnoreCommandException();
    }
    else if (restrict && ! dir.isChildOf(*restrict))
        throw Executor::IgnoreCommandException();
}

namespace {
    struct RemoveNodesFn
    {
        RemoveNodesFn(senf::console::DirectoryNode::ptr newNode) : newNode_ (newNode) {}

        bool operator()(senf::console::DirectoryNode::weak_ptr node) const
            { return node.expired() || node.lock()->isChildOf(*newNode_); }

        senf::console::DirectoryNode::ptr newNode_;
    };
}

prefix_ void senf::console::ConfigFile::insertParsedNode(DirectoryNode::ptr node)
{
    parsedNodes_.erase(
        std::remove_if(parsedNodes_.begin(), parsedNodes_.end(), RemoveNodesFn(node)),
        parsedNodes_.end());
    parsedNodes_.push_back(node);
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::console::readConfig(std::string const & filename, DirectoryNode & root)
{
    ConfigFile cfg (filename, root);
    cfg.parse();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Config.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
