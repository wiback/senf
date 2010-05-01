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
    \brief ProgramOptions unit tests */

//#include "ProgramOptions.test.hh"
//#include "ProgramOptions.test.ih"

// Custom includes
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
