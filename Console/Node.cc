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
    \brief Node non-inline non-template implementation */

#include "Node.hh"
//#include "Node.ih"

// Custom includes

//#include "Node.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::console::DirectoryNode & senf::console::root()
{
    static DirectoryNode::ptr rootNode(new DirectoryNode());
    return *rootNode;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::GenericNode

prefix_ std::string senf::console::GenericNode::path()
    const
{
    std::string path (name());
    ptr node (parent());
    while (node) {
        path = node->name() + "/" + path;
        node = node->parent();
    }
    return path.empty() ? "/" : path;
}

prefix_ bool senf::console::GenericNode::active()
    const
{
    cptr node (thisptr());
    while (node->parent())
        node = node->parent();
    return node == root().thisptr();
}

///////////////////////////////////////////////////////////////////////////
//senf::console::DirectoryNode

prefix_ senf::console::GenericNode::ptr
senf::console::DirectoryNode::remove(std::string const & name)
{
    ChildMap::iterator i (children_.find(name));
    if (i == children_.end()) 
        throw UnknownNodeNameException() << ": '" << name << "'";
    GenericNode::ptr node (i->second);
    children_.erase(i);
    node->parent_ = 0;
    return node;
}

prefix_ void senf::console::DirectoryNode::add(GenericNode::ptr node)
{
    BOOST_ASSERT( ! node->parent() );
    if (node->name().empty()) {
        node->name("unnamed");
        SENF_LOG((senf::log::MESSAGE)("Adding 'unnamed' node"));
    }
    if (children_.find(node->name()) != children_.end()) {
        unsigned suffix (0);
        std::string newName;
        do {
            ++suffix;
            newName = node->name() + "-" + boost::lexical_cast<std::string>(suffix);
        } while (children_.find(newName) != children_.end());
        SENF_LOG((senf::log::MESSAGE)("Uniquifying node '" << node->name() << "' to '" 
                                      << newName << "'"));
        node->name(newName);
    }
    children_.insert(std::make_pair(node->name(),node));
    node->parent_ = this;
}

prefix_ senf::console::GenericNode &
senf::console::DirectoryNode::get(std::string const & name)
    const
{
    ChildMap::const_iterator i (children_.find(name));
    if (i == children_.end())
        throw UnknownNodeNameException() << ": '" << name << "'";
    return *(i->second);
}

prefix_ void senf::console::DirectoryNode::v_help(std::ostream & output)
    const
{
    output << doc_ << "\n";
}

///////////////////////////////////////////////////////////////////////////
// senf::console::SimpleCommandNode

prefix_ void senf::console::SimpleCommandNode::v_help(std::ostream & output)
    const
{
    output << doc_ << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Node.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
