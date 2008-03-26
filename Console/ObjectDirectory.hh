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
    \brief ObjectDirectory public header */

#ifndef HH_ObjectDirectory_
#define HH_ObjectDirectory_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "Node.hh"

//#include "ObjectDirectory.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    template <class Owner, class Object>
    struct OwnerNodeCreateTraits
    {
        typedef BOOST_TYPEOF_TPL( senf_console_add_node( 
                                      * static_cast<DirectoryNode *>(0),
                                      * static_cast<Owner *>(0),
                                      * static_cast<std::string const *>(0),
                                      * static_cast<Object const *>(0)) ) result_type;

        typedef typename boost::remove_reference<result_type>::type NodeType;

        struct Creator {
            static NodeType & create(DirectoryNode & node, Owner & owner, 
                                     std::string const & name, Object const & ob);
        };
    };
    
    struct ObjectDirectoryBase {};

    /** \brief
      */
    template <class Owner>
    class ObjectDirectory : public ObjectDirectoryBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef Owner owner;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ObjectDirectory(Owner * owner);
        ~ObjectDirectory();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class Object>
        typename OwnerNodeCreateTraits<Owner, Object>::NodeType & add(std::string const & name,
                                                                      Object const & ob);

        DirectoryNode & node() const;

    protected:

    private:
        static SimpleCommandNode & create(DirectoryNode & node, Owner * owner, 
                                          std::string const & name, 
                                          SimpleCommandNode::Function const & fn);

        DirectoryNode::ptr node_;
        Owner * owner_;
    };

    template <class Owner, class Function>
    SimpleCommandNode & senf_console_add_node(
        DirectoryNode & node, Owner & owner, std::string const & name, Function const & fn);

    template <class Owner>
    SimpleCommandNode & senf_console_add_node(
        DirectoryNode & node, Owner & owner, std::string const & name, 
        void (Owner::*fn)(std::ostream & output, CommandNode::Arguments const & arguments));

    template <class Node>
    DirectoryNode & senf_console_add_node(
        DirectoryNode & dir, std::string const & name, Node const & node, int, 
        typename boost::enable_if< boost::is_convertible<Node*, ObjectDirectoryBase*> >::type * = 0);
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ObjectDirectory.cci"
//#include "ObjectDirectory.ct"
#include "ObjectDirectory.cti"
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
