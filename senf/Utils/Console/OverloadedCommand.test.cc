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
    \brief OverloadedCommand unit tests */

//#include "OverloadedCommand.test.hh"
//#include "OverloadedCommand.test.ih"

// Custom includes
#include <sstream>
#include "OverloadedCommand.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    void fn1(std::ostream &, senf::console::ParseCommandInfo const &)
    {
        throw senf::console::SyntaxErrorException("fn1 error");
    }

    void fn2(std::ostream &, senf::console::ParseCommandInfo const &)
    {
        throw senf::console::SyntaxErrorException("fn2 error");
    }

    void fn3(std::ostream & os, senf::console::ParseCommandInfo const &)
    {
        os << "fn3\n";
    }

}

BOOST_AUTO_UNIT_TEST(overladedCommand)
{
    senf::console::OverloadedCommandNode & cmd (
        senf::console::root()
            .add("overload", senf::console::OverloadedCommandNode::create())
            .doc("cmd") );
    cmd.add(senf::console::SimpleCommandOverload::create(&fn1)).doc("fn1");
    cmd.add(senf::console::SimpleCommandOverload::create(&fn2)).doc("fn2");

    {
        senf::console::ParseCommandInfo info;
        std::stringstream ss;
        BOOST_CHECK_THROW( senf::console::root()("overload")(ss, info),
                           senf::console::SyntaxErrorException );
        
        cmd.add(senf::console::SimpleCommandOverload::create(&fn3)).doc("fn3");
        SENF_CHECK_NO_THROW( senf::console::root()("overload")(ss, info) );
        BOOST_CHECK_EQUAL( ss.str(), "fn3\n" );
    }
    
    {
        std::stringstream ss;
        cmd.help(ss);
        BOOST_CHECK_EQUAL( ss.str(), 
                           "Usage:\n" 
                           "    1- overload ...\n"
                           "    2- overload ...\n"
                           "    3- overload ...\n"
                           "\n"
                           "cmd\n"
                           "\n"
                           "Variant 1:\n"
                           "fn1\n"
                           "\n"
                           "Variant 2:\n"
                           "fn2\n"
                           "\n"
                           "Variant 3:\n"
                           "fn3\n" );
    }

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
