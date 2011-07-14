// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Config non-inline non-template implementation */

#include "Config.hh"
#include "Config.ih"

// Custom includes
#include <senf/Utils/membind.hh>

//#include "Config.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::RestrictedExecutor

prefix_ senf::console::detail::RestrictedExecutor::RestrictedExecutor(DirectoryNode & root)
{
    executor_
        .chroot(root)
        .policy(senf::membind(&RestrictedExecutor::policyCallback, this));
}

prefix_ void
senf::console::detail::RestrictedExecutor::execute(std::ostream & output,
                                                   ParseCommandInfo const & command)
{
    executor_.execute(output, command);
}

prefix_ void
senf::console::detail::RestrictedExecutor::operator()(std::ostream & output,
                                                      ParseCommandInfo const & command)
{
    execute(output, command);
}

prefix_ senf::console::GenericNode &
senf::console::detail::RestrictedExecutor::getNode(ParseCommandInfo const & command)
{
    return executor_.getNode(command);
}

prefix_ bool senf::console::detail::RestrictedExecutor::parsed(GenericNode & node)
    const
{
    ParsedNodes::const_iterator i (parsedNodes_.begin());
    ParsedNodes::const_iterator const i_end (parsedNodes_.end());
    for (; i != i_end; ++i)
        if ( ! i->expired() && node.isChildOf(*(i->lock())) )
            return true;
    return false;
}

prefix_ void senf::console::detail::RestrictedExecutor::policyCallback(DirectoryNode & dir,
                                                                       std::string const & name)
{
    if (dir.hasChild(name)) {
        GenericNode & item (dir.get(name));
        if (restrict_ && ! item.isChildOf(*restrict_) && ! item.isDirectory())
            throw Executor::IgnoreCommandException();
        if (parsed(item))
            throw Executor::IgnoreCommandException();
    }
    else if (restrict_ && ! dir.isChildOf(*restrict_))
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

prefix_ void
senf::console::detail::RestrictedExecutor::insertParsedNode(DirectoryNode & node)
{
    parsedNodes_.erase(
        std::remove_if(parsedNodes_.begin(), parsedNodes_.end(), RemoveNodesFn(node.thisptr())),
        parsedNodes_.end());
    parsedNodes_.push_back(node.thisptr());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::ConfigBundle

prefix_ void senf::console::ConfigBundle::parse()
{
    detail::RestrictedExecutor::RestrictGuard guard (executor_);
    parseInternal();
}

prefix_ void senf::console::ConfigBundle::parse(DirectoryNode & restrict)
{
    detail::RestrictedExecutor::RestrictGuard guard (executor_, restrict);
    parseInternal();
}

prefix_ void senf::console::ConfigBundle::parseInternal()
{
    // It is valid to add additional sources at the end while parsing ...
    for (Sources::const_iterator i (sources_.begin()); i != sources_.end(); ++i)
        (*i)->parse(executor_);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::RestrictedExecutor::RestrictGuard

prefix_ senf::console::detail::RestrictedExecutor::RestrictGuard::
RestrictGuard(RestrictedExecutor & executor)
    : executor_ (executor)
{
    // This MUST BE root() not chroot() since restriction does NOT follow symlinks.
    // Therefore, if chroot() is a directory of symlinks, restricting to it will
    // execute NOTHING.
    executor_.restrict_ = console::root().thisptr();
}

prefix_ senf::console::detail::RestrictedExecutor::RestrictGuard::
RestrictGuard(RestrictedExecutor & executor, DirectoryNode & restrict)
    : executor_ (executor)
{
    executor_.restrict_ = restrict.thisptr();
}

prefix_ senf::console::detail::RestrictedExecutor::RestrictGuard::~RestrictGuard()
{
    if (! std::uncaught_exception())
        executor_.insertParsedNode( *executor_.restrict_ );
    executor_.restrict_ = console::root().thisptr();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::ConfigSource

prefix_ senf::console::detail::ConfigSource::~ConfigSource()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
