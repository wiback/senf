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
    \brief OverloadedCommand public header */

#ifndef HH_OverloadedCommand_
#define HH_OverloadedCommand_ 1

// Custom includes
#include "Node.hh"
#include <boost/intrusive_ptr.hpp>
#include "../Utils/intrusive_refcount.hh"

//#include "OverloadedCommand.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class OverloadedCommandNode;

    /** \brief Base class for command overload of OverloadedCommandNode

        This class is the base class of the commands which may be added to an
        OverloadedCommandNode.
      */
    class CommandOverload
        : public senf::intrusive_refcount
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<CommandOverload> ptr;
        typedef CommandNode::Arguments Arguments;

        ///////////////////////////////////////////////////////////////////////////

        virtual ~CommandOverload();

        void operator()(std::ostream & os, Arguments const & arguments);
                                        ///< Call the overload
                                        /**< If the \a arguments are not acceptable for this
                                             overload, a SyntaxErrorException must be thrown. */
        void help(std::ostream & os);   ///< Provide help for this specific overload

        OverloadedCommandNode & node(); ///< Access owning node
                                        /**< \pre The command \e must have been added to an
                                             OverloadedCommandNode. */

    protected:
        CommandOverload();

#ifndef DOXYGEN
    private:
#endif
        virtual void v_help(std::ostream & os) const = 0;
        virtual void v_execute(std::ostream & os, Arguments const & arguments) const = 0;

    private:
        OverloadedCommandNode * node_;

        friend class OverloadedCommandNode;
    };

    /** \brief Command node which allows multiple registered callbacks

        OverloadedCommandNode is like SimpleCommandNode but allows to register multiple commands to
        a single node. This works by calling each command in the list consecutively until no
        'SyntaxErrorException' exception is thrown.

        This works by first adding an OverloadedCommandNode to the directory in question and then
        adding commands to that node. Commands are derived from CommandOverload. 
        \code
        senf::console::DirectoryNode & dir (...);
        senf::console::OverloadedCommandNode & cmd (
            dir.add("cmd", senf::console::OverloadedCommandNode::create()) );
        cmd.add(senf::console::SimpleCommandOverload::create(&callback));
        cmd.add(senf::console::SimpleCommandOverload::create(&anotherCallback));
        \endcode

        However, this facility is mostly used not directly but indirectly (and automatically) when
        adding argument parsing callbacks.

        \warning For this to work, the commands <b>must</b> do all syntax checking before doing any
            operation

        \ingroup node_tree
      */
    class OverloadedCommandNode
        : public CommandNode
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<OverloadedCommandNode> ptr;
        typedef boost::shared_ptr<OverloadedCommandNode const> cptr;
        typedef boost::weak_ptr<OverloadedCommandNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static ptr create();

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        
        template <class Command>
        Command & add(boost::intrusive_ptr<Command> overload); ///< Add an additional overload

        ptr thisptr();
        cptr thisptr() const;

        OverloadedCommandNode & doc(std::string const & doc);
                                        ///< Assign global help for all overloads

    protected:

    private:
        OverloadedCommandNode();

        virtual void v_help(std::ostream & output) const;
        virtual void v_execute(std::ostream & output, Arguments const & arguments) const;

        typedef std::vector<CommandOverload::ptr> Overloads;

        Overloads overloads_;
        std::string doc_;
    };

    /** \brief Basic command overload

        This is an implementation of CommandOverload which allows to call an arbitrary callback with
        the correct signature (<tt>void (std::ostream &, Arguments const &)</tt>)
      */
    class SimpleCommandOverload
        : public CommandOverload
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<SimpleCommandOverload> ptr;
        typedef boost::function<void (std::ostream &, Arguments const &)> Function;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static SimpleCommandOverload::ptr create(Function fn);
                                        ///< Create new SimpleCommandOverload
                                        /**< \param[in] fn callback to call */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        SimpleCommandOverload & doc(std::string const & doc);
                                        ///< Assign overload specific documentation

    protected:

    private:
        SimpleCommandOverload(Function fn);

        virtual void v_help(std::ostream & os) const;
        virtual void v_execute(std::ostream & os, Arguments const & arguments) const;

        Function fn_;
        std::string doc_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "OverloadedCommand.cci"
//#include "OverloadedCommand.ct"
#include "OverloadedCommand.cti"
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
