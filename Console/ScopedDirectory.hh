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

#ifndef HH_ScopedDirectory_
#define HH_ScopedDirectory_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "Node.hh"

//#include "ScopedDirectory.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Internal: Node creation helper traits (ScopedDirectory proxy)
        
        This class is used like NodeCreateTraits to customize the child node creation. This trait
        class is used however by the ScopedDirectory proxy.
     */
    template <class Owner, class Object>
    struct OwnerNodeCreateTraits
    {
        typedef BOOST_TYPEOF_TPL( senf_console_add_node( 
                                      * static_cast<DirectoryNode *>(0),
                                      * static_cast<Owner *>(0),
                                      * static_cast<std::string const *>(0),
                                      * static_cast<Object const *>(0),
                                      * static_cast<int *>(0)) ) result_type;

        typedef typename boost::remove_reference<result_type>::type NodeType;

        /// Internal
        struct Creator {
            static NodeType & create(DirectoryNode & node, Owner & owner, 
                                     std::string const & name, Object const & ob);
        };
    };
    
    /** \brief Internal: Marker base class for all ScopedDirectory proxies
     */
    class ScopedDirectoryBase
    {
    public:
        DirectoryNode & node() const;   ///< Access the proxied DirectoryNode

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
    template <class Owner=void>
    class ScopedDirectory : public ScopedDirectoryBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef Owner owner;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ScopedDirectory(Owner * owner);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class Object>
        typename OwnerNodeCreateTraits<Owner, Object>::NodeType & add(std::string const & name,
                                                                      Object const & ob);
                                        ///< Create new child node
                                        /**< Adds a new child node to the (proxied)
                                             DirectoryNode. How the node is added is configured
                                             using the OwnerNodeCreateTraits template. The default
                                             implementation just forwards the call to the proxied
                                             directory node. */

    protected:

    private:
        Owner * owner_;
    };

    template <>
    class ScopedDirectory<void> : public ScopedDirectoryBase
    {
    public:
        template <class Object>
        typename NodeCreateTraits<Object>::NodeType & add(std::string const & name,
                                                          Object const & ob);
    };

#ifndef DOXYGEN
    template <class Owner, class Function>
    SimpleCommandNode & senf_console_add_node(
        DirectoryNode & node, Owner & owner, std::string const & name, Function const & fn, ...);

    template <class Owner>
    SimpleCommandNode & senf_console_add_node(
        DirectoryNode & node, Owner & owner, std::string const & name,
        void (Owner::*fn)(std::ostream &, ParseCommandInfo const &), int);

    template <class Node>
    DirectoryNode & senf_console_add_node(
        DirectoryNode & dir, std::string const & name, Node const & node, int, 
        typename boost::enable_if< boost::is_convertible<Node*, ScopedDirectoryBase*> >::type * = 0);
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
