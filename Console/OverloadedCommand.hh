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

    /** \brief
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
        void help(std::ostream & os);

        OverloadedCommandNode & node();

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

        OverloadedCommand is like SimpleCommand but allows to register multiple commands to a single
        node. This works by calling each command in the list consecutively until no 'SyntaxError'
        exception is thrown. 

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

        void add(CommandOverload::ptr overload);

        ptr thisptr();
        cptr thisptr() const;

        OverloadedCommandNode & doc(std::string const & doc);

    protected:

    private:
        OverloadedCommandNode();

        virtual void v_help(std::ostream & output) const;
        virtual void v_execute(std::ostream & output, Arguments const & arguments) const;

        typedef std::vector<CommandOverload::ptr> Overloads;

        Overloads overloads_;
        std::string doc_;
    };

    /** \brief
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

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        SimpleCommandOverload & doc(std::string const & doc);

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
//#include "OverloadedCommand.cti"
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
