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
    \brief ScopedDirectory public header */

#ifndef HH_SENF_Utils_Console_ScopedDirectory_
#define HH_SENF_Utils_Console_ScopedDirectory_ 1

// Custom includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "Node.hh"
#include "LazyDirectory.hh" // For ScopedDirectory template default arg

//#include "ScopedDirectory.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    namespace detail { struct OwnedNodeFactory {}; }

    /** \brief Internal: Marker base class for all ScopedDirectory proxies
     */
    class ScopedDirectoryBase
    {
    public:
        DirectoryNode & node() const;   ///< Access the proxied DirectoryNode
        operator DirectoryNode & () const; ///< Access the proxied DirectoryNode

        //-////////////////////////////////////////////////////////////////////////
        ///\name Proxied members (see DirectoryNode)
        //\{

        GenericNode::ptr remove(std::string const & name);
        bool hasChild(std::string const & name) const;
        DirectoryNode & getDirectory(std::string const & name) const;
        DirectoryNode & operator[](std::string const & name) const;
        CommandNode & getCommand(std::string const & name) const;
        CommandNode & operator()(std::string const & name) const;
        GenericNode & get(std::string const & name) const;
        DirectoryNode::ChildrenRange children() const;
        DirectoryNode & doc(std::string const & doc);
        std::string const & name() const;
        bool active() const;
        std::string path() const;
        std::string path(DirectoryNode const & root) const;
        boost::shared_ptr<DirectoryNode> parent() const;
        GenericNode::ptr unlink();
        void help(std::ostream & output) const;
        std::string shorthelp() const;

        //\}

    protected:
        ScopedDirectoryBase();
        ~ScopedDirectoryBase();

    private:
        DirectoryNode::ptr node_;
    };

    /** \brief DirectoryNode member proxy

        ScopedDirectory is used whenever a class wants to manage it's own directory. The class
        allows to declare the directory as a public member object. This allows the user of the class
        to register the directory in the command tree. By using the proxy, the node is automatically
        detached from the tree (and thereby destroyed) when the object (and thereby the proxy) is
        destroyed.

        \code
        class MyClass
        {
        public:
            ScopedDirectory<MyClass> configDir;

            MyClass() : configDir(this)
            {
                configDir.add(...);
            }
        };
        \endcode

        The ScopedDirectory proxy implements 'add()' to add new children to the proxied
        DirectoryNode. All add() variants supported by DirectoryNode are supported by
        ScopedDirectory.

        \idea This proxy could be made obsolete by allowing to allocate node objects
            statically. This could be achieved by moving back to an intrusive_ptr implementation for
            normal pointing needs with an added twist: Give each node a smart_ptr member pointing to
            itself with a null deleter. This allows to create weak_ptr's to the nodes which will
            automatically expire when the node is deleted (either statically or by the
            intrusive_ptr).

        \ingroup node_tree
      */
    template <class Owner>
    class ScopedDirectory : public ScopedDirectoryBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef Owner owner;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit ScopedDirectory(Owner * owner);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        template <class NodeType>
        NodeType & add(std::string const & name, boost::shared_ptr<NodeType> node);
        template <class NodeType>
        NodeType & add(std::string const & name, NodeType & node,
                       typename boost::enable_if< boost::is_convertible<NodeType &, GenericNode &> >::type * = 0);
        template <class Factory>
        typename Factory::result_type add(std::string const & name, Factory const & factory,
                                          typename boost::enable_if< boost::is_convertible<Factory*, detail::OwnedNodeFactory*> >::type * = 0);
        template <class Factory>
        typename Factory::result_type add(std::string const & name, Factory const & factory,
                                          typename boost::enable_if< boost::is_convertible<Factory*, detail::NodeFactory*> >::type * = 0,
                                          typename boost::disable_if< boost::is_convertible<Factory*, detail::OwnedNodeFactory*> >::type * = 0);

    protected:

    private:
        Owner * owner_;
    };

#ifndef DOXYGEN

    template <>
    class ScopedDirectory<void> : public ScopedDirectoryBase
    {
    public:
        template <class NodeType>
        NodeType & add(std::string const & name, boost::shared_ptr<NodeType> node);
        template <class NodeType>
        NodeType & add(std::string const & name, NodeType & node,
                       typename boost::enable_if< boost::is_convertible<NodeType &, GenericNode &> >::type * = 0);
        template <class Factory>
        typename Factory::result_type add(std::string const & name, Factory const & factory,
                                          typename boost::enable_if< boost::is_convertible<Factory*, detail::NodeFactory*> >::type * = 0);
    };

#endif

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "ScopedDirectory.cci"
//#include "ScopedDirectory.ct"
#include "ScopedDirectory.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
