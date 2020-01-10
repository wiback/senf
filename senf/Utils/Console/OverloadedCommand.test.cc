//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief OverloadedCommand unit tests */

// Custom includes
#include <sstream>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(overladedCommand)
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
