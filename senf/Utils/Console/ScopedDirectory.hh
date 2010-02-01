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
    \brief ScopedDirectory public header */

#ifndef HH_SENF_Scheduler_Console_ScopedDirectory_
#define HH_SENF_Scheduler_Console_ScopedDirectory_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "Node.hh"
#include "LazyDirectory.hh" // For ScopedDirectory template default arg

//#include "ScopedDirectory.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

        ///////////////////////////////////////////////////////////////////////////
        ///\name Proxied members (see DirectoryNode)
        ///\{

        GenericNode::ptr remove(std::string const & name);
        bool hasChild(std::string const & name) const;
        DirectoryNode & getDirectory(std::string const & name) const;
        DirectoryNode & operator[](std::string const & name) const;
        CommandNode & getCommand(std::string const & name) const;
        CommandNode & operator()(std::string const & name) const;
        GenericNode & get(std::string const & name) const;
        DirectoryNode & mkdir(std::string const & name);
        void link(std::string const & name, GenericNode & node);
        DirectoryNode::ChildrenRange children() const;
        DirectoryNode & doc(std::string const & doc);

        ///\}

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
                configDIr.add(...);
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
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef Owner owner;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit ScopedDirectory(Owner * owner);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
