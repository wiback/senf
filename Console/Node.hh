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

/** \defgroup node_tree The node tree
    
    The console/config node tree is the central data-structure of the library. Into this tree, all
    commands and parameters are entered. The tree is then exposed using a file-system like
    interface.
    
    \autotoc

    \section console_tree The tree

    We will start by giving a more complete example. This example contains most of the stuff needed
    for using the console/config library.

    \code
    // Define callback function.
    void mycommand(std::ostream & os, senf::console::ParseCommandInfo const & command)
    {
        // ...
        os << "!! Important message ...\n";
    }

    class SomeClass
    {
    public:
        // Declare a directory node (proxy) for use by this class. This must be public so we can add
        // it to the node tree later.
        senf::console::ScopedDirectory<SomeClass> dir;

        SomeClass() : dir(this) 
        {
            // You may document the directory here or later when adding it to the tree
            dir.doc("Manager for something");

            // Add a member function (the pointer-to-member is automatically bound to this instance)
            dir.add("member", &SomeClass::member)
                .doc("Do the member operation");
        }

        void member(std::ostream & os, senf::console::ParseCommandInfo const & command)
        {
            // ...
        }
    };

    int main(int, char**)
    {
        // Provide global documentation
        senf::console::root()
            .doc("This is someServer server");

        // Add a new directory to the root and document it. All the mutators return the node object
        // itself so operations can be chained.
        senf::console::DirectoryNode & mydir (
                .mkdir("myserver")
                .doc("My server specific directory"));

        // Add a command to that directory
        mydir.add("mycommand", &mycommand)
            .doc("mycommand <foo> [<bar>]\n\n"
                 "If <bar> is given, flurgle the <foo>, otherwise burgle it");

        // Create a SomeClass instance and add it's directory.
        SomeClass someClass;
        mydir.add("someClass", someClass.dir);

        // Start the interactive console server
        senf::console::Server::start(senf::INet4SocketAddress(senf::INet4Address::None, 23232u))
            .name("someServer");
    }
    \endcode

    \subsection console_nodes Node types

    The console/config library tree consists of two basic node types:
    
    \li senf::console::DirectoryNode provides internal nodes with an arbitrary number of children
    \li senf::console::CommandNode describes a command entry in the tree

    senf::console::CommandNode is the base-class of all command nodes of which there are several,
    depending on the type of command.

    There is a single root node, the senf::console::DirectoryNode called senf::console::root(). From
    this node, the tree is traversed.

    All nodes are allocated on the heap and are managed using a smart pointer.
    
    \subsection console_manipulate Manipulating the node tree

    There are several ways to add nodes to the tree:

    \li A senf::console::DirectoryNode can be added using senf::console::DirectoryNode::mkdir().
    \li An arbitrary node can be created and then (possibly later) added to the tree using the
        corresponding senf::console::DirectoryNode::add() overload.
    \li A senf::console::CommandNode is normally added to the tree by directly adding a callback
        using one of the overloaded senf::console::DirectoryNode::add() members. See \ref
        console_commands.

    When directly adding a node callback, the type of node added depends on the type of
    callback. The callback types which can be added are listed at \ref console_callbacks.
    
    \code
    void callback(std::ostream & os, senf::console::ParseCommandInfo const & command) { ... }
    // ...
    myDirectory.add("foo",&callback);
    \endcode

    Every node is identified among it's siblings by it's name. The name of the node is set when
    adding the node to the tree. If the name is empty or non-unique, a unique name will be
    automatically provided.

    To remove a node from the tree, just use the nodes senf::console::GenericNode::unlink() or the
    parents senf::console::DirectoryNode::remove() member. This call removes the node from it's
    parent and returns a (smart) node pointer.

    \li If you ignore the return value, the node (and it's children) will be deleted.
    \li Alternatively, you may store away the node and re-attach it later.
    \li An node (or subtree) can be moved to a different place by unlinking the node at it's old
        place and re-adding it at it's new location.
    \li To rename a node, unlink and re-add it with a different name.

    \code
    myDirectory.add("bar", myDirectory.remove("foo"));
    \endcode

    \subsection console_node_param Assigning additional node parameters

    Depending on the node type added, additional node parameters may be set. For example, every node
    has a documentation parameter which is used by the online-help system. To assign these
    parameters, the node exposes corresponding member functions. Since
    senf::console::DirectoryNode::add() returns the newly added node by reference, additional
    parameters may just be added to the end of the add command:
    \code
    myDirectory.add("foo",&fooCallback).doc("The foo method");
    \endcode
    Since the parameter setters all return the node reference, additional parameters may just be
    added to the end of the command.
    
    \subsection console_tree_traverse Traversing the tree

    The simplest way to access tree elements is to save the return value of the
    senf::console::DirectoryNode::add() members. However, saving the reference will not ensure, that
    the node is not removed. If the node might be removed from the tree, you should use a smart
    pointer (either <tt>ptr</tt> or <tt>weak_ptr</tt>) to hold the node.

    Another possibility is to traverse the tree explicitly. For this purpose, the operators '[]' and
    '()' have been overloaded in senf::console::DirectoryNode.
    \code
    senf::console::root().getDirectory("myDirectory").getCommand("foo")
    \\ or more concise but otherwise completely identical
    senf::console::root()["myDirectory"]("foo")
    \endcode

    getDirectory and the '[]' operator will return a senf::console::DirectoryNode whereas getCommand
    and the '()' operator will return a senf::console::CommandNode. If the node is not found or is
    not of the correct type, an exception will be raised.

    \section console_object_dir Assigning a directory to an object instance

    Most objects will register several commands. So it makes sense for these objects to manage their
    own directory. Since directories are however allocated on the heap, they cannot be directly
    added to a class. To facilitate this usage, the senf::console::ScopedDirectory is used. This
    class provides a senf::console::DirectoryNode facade. Internally, it automatically creates a
    senf::console::DirectoryNode to which all calls are forwarded.

    The senf::console::ScopedDirectory member should be declared public. This allows the user of the
    class to add the node to the tree.
 */

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
#include "../Utils/Logger/SenfLog.hh"
#include "../Utils/type_traits.hh"
#include "Parse.hh"

//#include "Node.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class DirectoryNode;
    class CommandNode;

    /** \brief Config/console node tree base-class

        GenericNode is the base class of all node objects. There are two basic node types derived
        from GenericNode:  DirectoryNode and CommandNode.

        All nodes are dynamically allocated and automatically managed using reference counting.

        All nodes are normally linked into a single tree which root node is
        senf::console::root(). Nodes may however be orphaned (not linked to the tree) either
        directly (the node has no parent) or indirectly (the node has a parent but is part of an
        orphaned subtree which is not linked to the root node).

        Every active (non-orphaned) node (except the root() node) has a non-empty node name. This
        name is assigned to the node when adding the node to the tree.

        \ingroup node_tree
      */
    class GenericNode 
        : public boost::enable_shared_from_this<GenericNode>
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<GenericNode> ptr;
        typedef boost::shared_ptr<GenericNode const> cptr;
        typedef boost::weak_ptr<GenericNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

        virtual ~GenericNode();

        std::string const & name() const; ///< Node name
        boost::shared_ptr<DirectoryNode> parent() const; ///< Parent node
                                        /**< May be null, if the node is the root node or if it is
                                             not linked to the tree */

        std::string path() const;       ///< Node path
                                        /**< The node path is built by joining the names of all
                                             parent nodes with '/' chars. */

        ptr unlink();                   ///< Remove node from it's parent directory
                                        /**< You may either discard the return value and thereby
                                             dispose the node or may re-attach the node at some
                                             other place using DirectoryNode::add(). */

        bool active() const;            ///< \c true, if node is attached to the root() node

        void help(std::ostream & output) const; /// Write help info to \a output

        ptr thisptr();                  ///< Get smart pointer to node
        cptr thisptr() const;           ///< Get smart pointer to node (const)

    protected:
        GenericNode();

        void name(std::string const & name);

#ifndef DOXYGEN
    private:
#else
    public:
#endif
        virtual void v_help(std::ostream & output) const = 0;
                                        ///< Provide help information
                                        /**< This member must be implemented in derived classes
                                             to provide node specific help information. */

    private:
        std::string name_;
        DirectoryNode * parent_;

        friend class DirectoryNode;
    };

    class SimpleCommandNode;

    /** \brief Internal: Node creation helper traits
        
        This class is used internally to find out the type of node to create for a specific argument
        type. 
     */
    template <class Object>
    struct NodeCreateTraits
    {
        typedef BOOST_TYPEOF_TPL( senf_console_add_node( 
                                      * static_cast<DirectoryNode *>(0),
                                      * static_cast<std::string const *>(0),
                                      * static_cast<Object *>(0),
                                      0) ) base_type;
        typedef typename senf::remove_cvref<base_type>::type value_type;

        typedef typename value_type::node_type NodeType;
        typedef typename value_type::return_type result_type;

        /// Internal
        struct Creator {
            static result_type create(DirectoryNode & node, std::string const & name, 
                                      Object & ob);
        };
    };

    /** \brief Config/console tree directory node

        This node type provides the internal and root nodes of the tree. It allows to add arbitrary
        children and supports directory traversal.
        
        Nodes are normally not instantiated manually but are created by the DirectoryNode via
        mkdir() or add(). Special add() members however allow externally allocated node objects.

        Nodes may be added to the tree only once, otherwise chaos will ensue. Since nodes are always
        managed dynamically, there is a special ScopedDirectory proxy template which provides a
        DirectoryNode facade. ScopedDirectory is used if a class wants to manage it's own directory
        as a data member.

        Every node is assigned a (new) name when it is added to a directory. If the directory
        already has an entry of that name, the name is made unique by appending a suffix of the form
        '-n' where n is a number starting at 1. If the name is empty, int is set to 'unnamed' and
        then uniquified as above. Automatically providing unique names simplifies adding
        configuration/console support to generic components.

        \ingroup node_tree
      */
    class DirectoryNode : public GenericNode
    {
        SENF_LOG_CLASS_AREA();
        typedef std::map<std::string, GenericNode::ptr> ChildMap;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<DirectoryNode> ptr;
        typedef boost::shared_ptr<DirectoryNode const> cptr;
        typedef boost::weak_ptr<DirectoryNode> weak_ptr;

        typedef boost::iterator_range<ChildMap::const_iterator> ChildrenRange;
        typedef ChildMap::const_iterator child_iterator;

        typedef DirectoryNode node_type;
        typedef DirectoryNode & return_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{

        static ptr create();            ///< Create node object.
                                        /**< You should normally use either mkdir() or
                                             ScopedDirectory instead of create() */

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Children
        ///\{

        template <class NodeType>
        NodeType & add(std::string const & name, boost::shared_ptr<NodeType> node);
                                        ///< Add node to tree
                                        /**< Adds the \a node to the tree as a child of \a this
                                             node. The node is given the name \a name. If a node of
                                             that name already exists, a numeric suffix of the form
                                             '-n' is added to the name until the name is unique. If
                                             \a name is empty, it is set to 'unnamed'. */

        template <class Object>
        typename NodeCreateTraits<Object>::result_type add(std::string const & name, 
                                                           Object const & ob);
                                        ///< Generic child node factory
                                        /**< This member is used to create a new child node of the
                                             current directory. The type of node created depends on
                                             the type of argument passed.

                                             The node type is selected by the NodeCreateTraits
                                             class. To allow adding a specific node type, you need
                                             to provide an overload for
                                             <tt>senf_console_add_node</tt> which must be visible at
                                             when you register the new node.
                                             \code
                                             MyNodeType & senf_console_add_node(
                                                 DirectoryNode & dir,
                                                 std::string const & name,
                                                 MySpecialObject const & ob,
                                                 int)
                                             {
                                                 return dir.add(name, MyNodeType::create(ob));
                                             }
                                             \endcode
                                             (Do not forget the last unnamed 'int' parameter which
                                             is not used but serves to disambiguate the
                                             overloads). */

        template <class Object>
        typename NodeCreateTraits<Object>::result_type add(std::string const & name, 
                                                           Object & ob);
                                        ///< Generic child node factory
                                        /**< \see add() */

        GenericNode::ptr remove(std::string const & name);
                                        ///< Remove node \a name from the tree
                                        /**< The returned pointer may either be discarded, which
                                             will automatically dispose the removed node, or it may
                                             be saved and/or re-attached at some other place in the
                                             tree. */

        bool hasChild(std::string const & name) const;
                                        ///< \c true, if there is a child with name \a name

        GenericNode & get(std::string const & name) const;
                                        ///< Get child node
                                        /**< \throws UnknownNodeNameException if a child \a name
                                                 does not exist */

        DirectoryNode & getDirectory(std::string const & name) const;
                                        ///< Get directory child node
                                        /**< Same as operator[]
                                             \throws UnknownNodeNameException if a child \a name
                                                 does not exist. 
                                             \throws std::bad_cast if the child \a name is not a
                                                 directory node. */
        
        DirectoryNode & operator[](std::string const & name) const;
                                        ///< Get directory child node
                                        /**< Same as getDirectory
                                             \throws UnknownNodeNameException if a child \a name
                                                 does not exist. 
                                             \throws std::bad_cast if the child \a name is not a
                                                 directory node. */

        CommandNode & getCommand(std::string const & name) const;
                                        ///< Get command child node
                                        /**< Same as operator()
                                             \throws UnknownNodeNameException if a child \a name
                                                 does not exist
                                             \throws std::bad_cast if the child \a name is not a
                                                 command node. */

        CommandNode & operator()(std::string const & name) const;
                                        ///< Get command child node
                                        /**< Same as getCommand()
                                             \throws UnknownNodeNameException if a child \a name
                                                 does not exist
                                             \throws std::bad_cast if the child \a name is not a
                                                 command node. */

        DirectoryNode & mkdir(std::string const & name);
                                        ///< Create sub-directory node
        
        ChildrenRange children() const; ///< Return iterator range over all children.
                                        /**< The returned range is sorted by child name. */

        ChildrenRange completions(std::string const & s) const;
                                        ///< Return iterator range of completions for \a s
                                        /**< The returned range is sorted by child name. */

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        template <class ForwardRange>
        GenericNode & traverse(ForwardRange const & range, bool autocomplete=false);
                                        ///< Traverse node path starting at this node
                                        /**< The <tt>ForwardRange::value_type</tt> must be
                                             (convertible to) std::string. Each range element
                                             constitutes a step along the node traversal.

                                             If the range starts with an empty element, the
                                             traversal is started at the root() node, otherwise it
                                             is started at \a this node. The traversal supports '.',
                                             '..' and ignores further empty elements. 

                                             If \a autocomplete is set to \c true, invalid path
                                             components which can be uniquely completed will be
                                             completed automatically while traversing the tree. */

        DirectoryNode & doc(std::string const & doc);
                                        ///< Set node documentation

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

    /// Exception: Unknown node name
    struct UnknownNodeNameException : public senf::Exception
    { UnknownNodeNameException() : senf::Exception("Unknown node name") {}};

#ifndef DOXYGEN
    template <class Type>
    struct NodeCreateTraits< boost::shared_ptr<Type> >
    {};
#endif

    /** \brief Config/console tree command node

        The CommandNode is the base-class for the tree leaf nodes. Concrete command node
        implementations are derived from this class.

        To execute a command, CommandNode::operator()() or CommandNode::execute() is called.

        Subclass instances of this node type are automatically created when adding commands to the
        tree. See \ref console_commands.

        \ingroup node_tree
      */
    class CommandNode : public GenericNode
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<CommandNode> ptr;
        typedef boost::shared_ptr<CommandNode const> cptr;
        typedef boost::weak_ptr<CommandNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

        void execute(std::ostream & output, ParseCommandInfo const & command) const;
                                        ///< Execute the command
                                        /**< Same as operator()()
                                             \param[in] output stream where result messages may be
                                                 written to
                                             \param[in] arguments command arguments. This is a
                                                 range of ranges of ArgumentToken instances. */

        void operator()(std::ostream & output, ParseCommandInfo const & command) const;
                                        ///< Execute the command
                                        /**< Same as execute()
                                             \param[in] output stream where result messages may be
                                                 written to
                                             \param[in] arguments command arguments. This is a
                                                 range of ranges of ArgumentToken instances. */

        ptr thisptr();
        cptr thisptr() const;

    protected:
        CommandNode();

#ifndef DOXYGEN
    private:
#endif
        virtual void v_execute(std::ostream & output, ParseCommandInfo const & command) const = 0;
                                        ///< Called to execute the command
                                        /**< \param[in] output stream where result messages may be
                                                 written to
                                             \param[in] arguments command arguments. This is a
                                                 range of ranges of ArgumentToken instances. */

    private:
    };

    /** \brief Most simple CommandNode implementation

        This CommandNode implementation simply forwards the \a output and \a arguments arguments to
        an arbitrary callback. Thus, it allows to add callbacks with the signature
        \code
        void callback(std::ostream & os, senf::console::ParseCommandInfo const & command)
        { ... }
        \endcode
        to the tree.
 
        \ingroup console_commands
     */
    class SimpleCommandNode : public CommandNode
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<SimpleCommandNode> ptr;
        typedef boost::shared_ptr<SimpleCommandNode const> cptr;
        typedef boost::weak_ptr<SimpleCommandNode> weak_ptr;

        typedef boost::function<void (std::ostream &, ParseCommandInfo const &)> Function;

        typedef SimpleCommandNode node_type;
        typedef SimpleCommandNode & return_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{

        static ptr create(Function const & fn);

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        ptr thisptr();
        cptr thisptr() const;

        SimpleCommandNode & doc(std::string const & doc);

    protected:
        SimpleCommandNode(Function const & fn);

    private:
        virtual void v_help(std::ostream & output) const;
        virtual void v_execute(std::ostream & output, ParseCommandInfo const & command) const;
        

        Function fn_;
        std::string doc_;
    };

#ifndef DOXYGEN

    SimpleCommandNode & senf_console_add_node(DirectoryNode & node, std::string const & name, 
                                              SimpleCommandNode::Function fn, int);

#endif

    DirectoryNode & root();

}}

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TYPE(senf::console::DirectoryNode)
BOOST_TYPEOF_REGISTER_TYPE(senf::console::SimpleCommandNode)


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
