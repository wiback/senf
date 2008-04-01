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
    \brief OverloadedCommand.test unit tests */

//#include "OverloadedCommand.test.hh"
//#include "OverloadedCommand.test.ih"

// Custom includes
#include <sstream>
#include "OverloadedCommand.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    void fn1(std::ostream &, senf::console::CommandOverload::Arguments const &)
    {
        throw senf::console::SyntaxErrorException("fn1 error");
    }

    void fn2(std::ostream &, senf::console::CommandOverload::Arguments const &)
    {
        throw senf::console::SyntaxErrorException("fn2 error");
    }

    void fn3(std::ostream & os, senf::console::CommandOverload::Arguments const &)
    {
        os << "fn3\n";
    }

}

BOOST_AUTO_UNIT_TEST(overladedCommand)
{
    senf::console::OverloadedCommandNode & cmd (
        senf::console::root().add("overload", senf::console::OverloadedCommandNode::create()));
    cmd.add(senf::console::SimpleCommandOverload::create(&fn1));
    cmd.add(senf::console::SimpleCommandOverload::create(&fn2));

    senf::console::ParseCommandInfo info;
    std::stringstream ss;
    BOOST_CHECK_THROW( senf::console::root()("overload")(ss, info.arguments()),
                       senf::console::SyntaxErrorException );

    cmd.add(senf::console::SimpleCommandOverload::create(&fn3));
    BOOST_CHECK_NO_THROW( senf::console::root()("overload")(ss, info.arguments()) );
    BOOST_CHECK_EQUAL( ss.str(), "fn3\n" );

    cmd.unlink();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
