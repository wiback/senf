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
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include "../Utils/Exception.hh"
#include "../Utils/mpl.hh"
#include "Parse.hh"

//#include "Node.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

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
        typedef boost::shared_ptr<GenericNode const> cptr;
        typedef boost::weak_ptr<GenericNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

        virtual ~GenericNode();

        std::string const & name() const;
        boost::shared_ptr<DirectoryNode> parent() const;

        std::string path() const;

        ptr unlink();

        bool active() const;

        void help(std::ostream & output) const;

        ptr thisptr();
        cptr thisptr() const;

    protected:
        GenericNode();

        void name(std::string const & name);
        static void name(GenericNode & node, std::string const & name);
        void parent(DirectoryNode * parent);

    private:
        virtual void v_help(std::ostream & output) const = 0;

        std::string name_;
        DirectoryNode * parent_;

        friend class intrusive_refcount_base;
        friend class DirectoryNode;
    };

    class SimpleCommandNode;

    template <class Object>
    struct NodeCreateTraits
    {
        typedef BOOST_TYPEOF_TPL( senf_console_add_node( 
                                      * static_cast<DirectoryNode *>(0),
                                      * static_cast<std::string const *>(0),
                                      * static_cast<Object const *>(0),
                                      0) ) result_type;

        typedef typename boost::remove_reference<result_type>::type NodeType;

        struct Creator {
            static NodeType & create(DirectoryNode & node, std::string const & name, 
                                     Object const & ob);
        };
    };

    /** \brief
        ///\fixme Provide a default name for added nodes if 'name' is empty ?
      */
    class DirectoryNode : public GenericNode
    {
        typedef std::map<std::string, GenericNode::ptr> ChildMap;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<DirectoryNode> ptr;
        typedef boost::shared_ptr<DirectoryNode const> cptr;
        typedef boost::weak_ptr<DirectoryNode> weak_ptr;

        typedef boost::iterator_range<ChildMap::const_iterator> ChildrenRange;
        typedef ChildMap::const_iterator child_iterator;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{

        static std::auto_ptr<DirectoryNode> create();

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Children
        ///\{

        template <class NodeType>
        NodeType & add(std::string const & name, std::auto_ptr<NodeType> node);

        template <class NodeType>
        NodeType & add(std::string const & name, boost::shared_ptr<NodeType> node);

        template <class Object>
        typename NodeCreateTraits<Object>::NodeType & add (std::string const & name, 
                                                           Object const & ob);

        GenericNode::ptr remove(std::string const & name);

        DirectoryNode & operator[](std::string const & name) const;
        CommandNode & operator()(std::string const & name) const;
        GenericNode & get(std::string const & name) const;

        DirectoryNode & mkdir(std::string const & name);
        
        ChildrenRange children() const;

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        template <class ForwardRange>
        GenericNode & traverse(ForwardRange const & range);

        DirectoryNode & doc(std::string const & doc);

        ptr thisptr();
        cptr thisptr() const;

    protected:
        DirectoryNode();

    private:
        void add(GenericNode::ptr node);
        virtual void v_help(std::ostream & output) const;

        ChildMap children_;
        std::string doc_;

        friend DirectoryNode & root();
    };

    BOOST_TYPEOF_REGISTER_TYPE(DirectoryNode);

    struct UnknownNodeNameException : public senf::Exception
    { UnknownNodeNameException() : senf::Exception("Unknown node name") {}};

    // We need this specialization since we cannot passe auto_ptr via const & !!
    template <class Type>
    struct NodeCreateTraits< std::auto_ptr<Type> >
    {};

    template <class Type>
    struct NodeCreateTraits< boost::shared_ptr<Type> >
    {};

    /** \brief
      */
    class CommandNode : public GenericNode
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<CommandNode> ptr;
        typedef boost::shared_ptr<CommandNode const> cptr;
        typedef boost::weak_ptr<CommandNode> weak_ptr;

        typedef ParseCommandInfo::ArgumentsRange Arguments;

        ///////////////////////////////////////////////////////////////////////////

        virtual void operator()(std::ostream & output, Arguments const & arguments) = 0;

        ptr thisptr();
        cptr thisptr() const;

    protected:
        CommandNode();

    private:
    };

    /** \brief
      */
    class SimpleCommandNode : public CommandNode
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (std::ostream &, Arguments const &)> Function;

        ///////////////////////////////////////////////////////////////////////////

        virtual void operator()(std::ostream & output, Arguments const & arguments);

        ptr thisptr();
        cptr thisptr() const;

        static std::auto_ptr<SimpleCommandNode> create(Function const & fn);

        SimpleCommandNode & doc(std::string const & doc);

    protected:
        SimpleCommandNode(Function const & fn);

    private:
        virtual void v_help(std::ostream & output) const;

        Function fn_;
        std::string doc_;
    };

    template <class Function>
    SimpleCommandNode & senf_console_add_node(DirectoryNode & node, std::string const & name, 
                                              Function const & fn, ...);

    BOOST_TYPEOF_REGISTER_TYPE(SimpleCommandNode);

    DirectoryNode & root();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Node.cci"
#include "Node.ct"
#include "Node.cti"
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
