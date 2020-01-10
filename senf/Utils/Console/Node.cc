//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Node non-inline non-template implementation */

#include "Node.hh"

// Custom includes
#include <senf/Utils/Range.hh>

//#include "Node.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::console::DirectoryNode & senf::console::root()
{
    static DirectoryNode::ptr rootNode (new DirectoryNode());
    return *rootNode;
}

namespace {
    void dodump(std::ostream & output, unsigned level, senf::console::DirectoryNode & node)
    {
        std::string pad (2*level, ' ');
        senf::console::DirectoryNode::child_iterator i (node.children().begin());
        senf::console::DirectoryNode::child_iterator const i_end (node.children().end());
        for (; i != i_end; ++i) {
            output << pad << i->first;
            if (i->second->isDirectory()) {
                output << "/\n";
                dodump(output, level+1,static_cast<senf::console::DirectoryNode&>(*i->second));
            }
            else if (i->second->isLink())
                output << "@ -> " << i->second->followLink().path() << '\n';
            else
                output << '\n';
        }
    }
}

prefix_ void senf::console::dump(std::ostream & os, DirectoryNode & dir)
{
    dodump(os,0,dir);
}

prefix_ void senf::console::moveChildren(DirectoryNode & from, DirectoryNode & to)
{
    DirectoryNode::child_iterator child (from.children().begin());
    while (child != from.children().end()) {
        std::string name (child->first);
        ++child;
        to.add(name, from.remove(name));
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

prefix_ std::string senf::console::GenericNode::path(DirectoryNode const & root)
    const
{
    std::string path;
    cptr node (thisptr());
    while (node && node != root.thisptr()) {
        if (! path.empty())
            path = node->name() + "/" + path;
        else
            path = node->name();
        node = node->parent();
    }
    if (path.empty() || path[0] != '/')
        path = "/" + path;
    return path;
}

prefix_ bool senf::console::GenericNode::active()
    const
{
    cptr node (thisptr());
    while (node->parent())
        node = node->parent();
    return node == root().thisptr();
}

prefix_ bool senf::console::GenericNode::isChildOf(DirectoryNode & parent)
    const
{
    cptr node (thisptr());
    while (node && node != parent.thisptr())
        node = node->parent();
    return node.get() != 0;
}

prefix_ void senf::console::GenericNode::rename(std::string const & newName)
{
    DirectoryNode::ptr myParent (parent());
    if (myParent)
        myParent->add( newName, unlink());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::LinkNode

prefix_ void senf::console::LinkNode::v_help(std::ostream & os)
    const
{
    follow().help(os);
}

prefix_ std::string senf::console::LinkNode::v_shorthelp()
    const
{
    return follow().shorthelp();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//senf::console::DirectoryNode

prefix_ senf::console::DirectoryNode::~DirectoryNode()
{
    ChildMap::iterator i (children_.begin());
    ChildMap::iterator const i_end (children_.end());
    for (; i != i_end; ++i)
        i->second->parent_ = 0;
}

prefix_ senf::console::GenericNode::ptr
senf::console::DirectoryNode::remove(std::string const & name, NoThrow_t)
{
    try {
        return remove( name);
    } catch (UnknownNodeNameException &) {}
    return ptr();
}

prefix_ senf::console::GenericNode::ptr
senf::console::DirectoryNode::remove(std::string const & name)
{
    ChildMap::iterator i (children_.find(name));
    if (i == children_.end())
        throw UnknownNodeNameException() << ": '" << name << "'";
    GenericNode::ptr node (i->second);
    children_.erase(i);
    node->parent_ = 0;
    node->name_.clear();
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
senf::console::DirectoryNode::getLink(std::string const & name)
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

prefix_ std::string senf::console::DirectoryNode::v_shorthelp()
    const
{
    if (! shortdoc_.empty())
        return shortdoc_;
    return doc_.substr(0,doc_.find('\n'));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::SimpleCommandNode

prefix_ void senf::console::SimpleCommandNode::v_help(std::ostream & output)
    const
{
    output << doc_ << "\n";
}

prefix_ std::string senf::console::SimpleCommandNode::v_shorthelp()
    const
{
    if (! shortdoc_.empty())
        return shortdoc_;
    return doc_.substr(0,doc_.find('\n'));
}

prefix_ void senf::console::SimpleCommandNode::v_execute(boost::any & rv, std::ostream & os,
                                                         ParseCommandInfo const & command)
    const
{
    fn_(os, command);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
