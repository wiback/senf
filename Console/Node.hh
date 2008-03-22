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
    \brief Node public header */

#ifndef HH_Node_
#define HH_Node_ 1

// Custom includes
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/range/iterator_range.hpp>
#include "../Utils/Exception.hh"

//#include "Node.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class DirectoryNode;
    class CommandNode;

    /** \brief
      */
    class GenericNode 
        : public boost::enable_shared_from_this<GenericNode>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<GenericNode> ptr;
        typedef boost::weak_ptr<GenericNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

        virtual ~GenericNode();

        std::string const & name() const;
        boost::shared_ptr<DirectoryNode> parent() const;
        bool managed() const;

        std::string path() const;

    protected:
        explicit GenericNode(std::string const & name);

        void name(std::string const & name);
        static void name(GenericNode & node, std::string const & name);
        void parent(DirectoryNode * parent);

    private:
        std::string name_;
        DirectoryNode * parent_;

        friend class intrusive_refcount_base;
        friend class DirectoryNode;
    };

    /** \brief
      */
    class DirectoryNode : public GenericNode
    {
        typedef std::map<std::string, GenericNode::ptr> ChildMap;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<DirectoryNode> ptr;
        typedef boost::weak_ptr<DirectoryNode> weak_ptr;

        typedef boost::iterator_range<ChildMap::const_iterator> ChildrenRange;
        typedef ChildMap::const_iterator child_iterator;

        ///////////////////////////////////////////////////////////////////////////

        GenericNode & add(std::auto_ptr<GenericNode> node, bool uniquify = true);

        DirectoryNode & operator[](std::string const & name) const;
        CommandNode & operator()(std::string const & name) const;

        DirectoryNode & mkdir(std::string const & name);
        
        ChildrenRange children() const;

    protected:
        explicit DirectoryNode(std::string const & name);

    private:
        void add(GenericNode::ptr node, bool uniquify);
        GenericNode & lookup(std::string const & name) const;

        ChildMap children_;

        friend DirectoryNode & root();
    };

    struct DuplicateNodeNameException : public senf::Exception
    { DuplicateNodeNameException() : senf::Exception("Duplicate node name") {}};

    struct UnknownNodeNameException : public senf::Exception
    { UnknownNodeNameException() : senf::Exception("Unknown node name") {}};

    /** \brief
      */
    class CommandNode : public GenericNode
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<CommandNode> ptr;
        typedef boost::weak_ptr<CommandNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

    protected:
        explicit CommandNode(std::string const & name);

    private:

    };

    DirectoryNode & root();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Node.cci"
//#include "Node.ct"
//#include "Node.cti"
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
