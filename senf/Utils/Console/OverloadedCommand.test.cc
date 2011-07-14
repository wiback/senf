// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief OverloadedCommand unit tests */

//#include "OverloadedCommand.test.hh"
//#include "OverloadedCommand.test.ih"

// Custom includes
#include <sstream>
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_UNIT_TEST(overladedCommand)
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
