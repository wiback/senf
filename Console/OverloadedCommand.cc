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
    \brief OverloadedCommand non-inline non-template implementation */

#include "OverloadedCommand.hh"
//#include "OverloadedCommand.ih"

// Custom includes

//#include "OverloadedCommand.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::OverloadedCommandNode

prefix_ void senf::console::OverloadedCommandNode::v_help(std::ostream & os)
    const
{
    os << doc_;
    Overloads::const_iterator i (overloads_.begin());
    Overloads::const_iterator const i_end (overloads_.end());
    for (; i != i_end; ++i) {
        os << "\n\n";
        (*i)->help(os);
    }
}

prefix_ void senf::console::OverloadedCommandNode::v_execute(std::ostream & output,
                                                             Arguments const & arguments)
    const
{
    Overloads::const_iterator i (overloads_.begin());
    Overloads::const_iterator const i_end (overloads_.end());
    SyntaxErrorException err;
    for (; i != i_end; ++i) {
        try {
            (**i)(output, arguments);
            return;
        }
        catch (SyntaxErrorException & ex) {
            err = ex;
        };
    }
    throw err;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::SimpleCommandOverload

prefix_ void senf::console::SimpleCommandOverload::v_help(std::ostream & os)
    const
{
    os << doc_;
}

prefix_ void senf::console::SimpleCommandOverload::v_execute(std::ostream & os,
                                                             Arguments const & arguments)
    const
{
    fn_(os, arguments);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "OverloadedCommand.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
