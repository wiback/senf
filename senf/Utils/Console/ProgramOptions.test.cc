//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ProgramOptions unit tests */

// Custom includes
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(programOptions)
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
