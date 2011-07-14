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
    \brief ProgramOptions unit tests */

//#include "ProgramOptions.test.hh"
//#include "ProgramOptions.test.ih"

// Custom includes
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    std::string val1;
    bool val2 (false);

    void fun1(std::string p)
    { val1 = p; }

    void fun2()
    { val2 = true; }
}

SENF_AUTO_UNIT_TEST(programOptions)
{
    namespace fty = senf::console::factory;

    senf::console::ScopedDirectory<> root;
    senf::console::root().add("root", root);

    root.add("dir1", fty::Directory()).add("fun1", fty::Command(&fun1));
    root.add("fun2", fty::Command(&fun2));
    root.add("name-with-dashes", fty::Directory()).add("fun-2", fty::Command(&fun2));

    {
        char const * argv[] = { "", "--dir1-fun1=foo","--fun2" };
        senf::console::ProgramOptions opts (sizeof(argv)/sizeof(argv[0]), argv, root);

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "foo" );
        BOOST_CHECK_EQUAL( val2, true );
    }

    {
        char const * argv[] = { "", "--d-f=foo","--fun" };
        senf::console::ProgramOptions opts (sizeof(argv)/sizeof(argv[0]), argv, root);

        val1 = "";
        val2 = false;

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "foo" );
        BOOST_CHECK_EQUAL( val2, true );
    }

    {
        char const * argv[] = { "", "--name-w-fun" };
        senf::console::ProgramOptions opts (sizeof(argv)/sizeof(argv[0]), argv, root);

        val1 = "";
        val2 = false;

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "" );
        BOOST_CHECK_EQUAL( val2, true );
    }

    {
        char const * argv[] = { "", "-ab" };
        senf::console::ProgramOptions opts(sizeof(argv)/sizeof(argv[0]), argv, root);
        opts
            .alias('a', "--dir1-fun1=baz")
            .alias('b', "--fun2");

        val1 = "";
        val2 = false;

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "baz" );
        BOOST_CHECK_EQUAL( val2, true );
    }

    {
        char const * argv[] = { "", "-badoo" };
        senf::console::ProgramOptions opts(sizeof(argv)/sizeof(argv[0]), argv, root);
        opts
            .alias('a', "--dir1-fun1", true)
            .alias('b', "--fun2");

        val1 = "";
        val2 = false;

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "doo" );
        BOOST_CHECK_EQUAL( val2, true );
    }

    {
        char const * argv[] = { "", "-a","dii","-b" };
        senf::console::ProgramOptions opts(sizeof(argv)/sizeof(argv[0]), argv, root);
        opts
            .alias('a', "--dir1-fun1", true)
            .alias('b', "--fun2");

        val1 = "";
        val2 = false;

        SENF_CHECK_NO_THROW( opts.parse() );
        BOOST_CHECK_EQUAL( val1, "dii" );
        BOOST_CHECK_EQUAL( val2, true );
    }
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
