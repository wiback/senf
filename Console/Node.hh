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
#include <boost/intrusive_ptr.hpp>
#include <boost/utility.hpp>
#include "../Utils/intrusive_refcount.hh"
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
        : public intrusive_refcount_t<GenericNode>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<GenericNode> ptr;

        ///////////////////////////////////////////////////////////////////////////

        std::string const & name() const;
        DirectoryNode & parent() const;
        bool managed() const;

    protected:
        explicit GenericNode(std::string const & name, bool managed = false);

        void name(std::string const & name);
        static void name(GenericNode & node, std::string const & name);
        void parent(DirectoryNode * parent);

    private:
        bool release();

        std::string name_;
        bool managed_;
        DirectoryNode * parent_;

        friend class intrusive_refcount_base;
    };

    /** \brief
      */
    class DirectoryNode : public GenericNode
    {
    public:
        typedef boost::intrusive_ptr<DirectoryNode> ptr;

        void add(std::auto_ptr<GenericNode> node, bool uniquify = true);
        void add(GenericNode & node, bool uniquify = true);

        DirectoryNode & operator[](std::string const & name) const;
        CommandNode & operator()(std::string const & name) const;

    protected:
        explicit DirectoryNode(std::string const & name, bool managed = false);

    private:
        void add(GenericNode::ptr node, bool uniquify);
        GenericNode & lookup(std::string const & name) const;

        typedef std::map<std::string, GenericNode::ptr> ChildMap;
        ChildMap children_;
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
        typedef boost::intrusive_ptr<CommandNode> ptr;

    protected:
        explicit CommandNode(std::string const & name, bool managed = false);

    private:

    };

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
