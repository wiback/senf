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
    \brief Executor.test unit tests */

//#include "Executor.test.hh"
//#include "Executor.test.ih"

// Custom includes
#include <sstream>
#include <vector>
#include "Executor.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    std::vector<senf::console::ParseCommandInfo> commands;
    void setCommand(senf::console::ParseCommandInfo const & cmd) {
        commands.push_back(cmd);
    }
    void testCommand(std::ostream & os, senf::console::ParseCommandInfo const & command) {
        os << "testCommand\n";
    }
}

BOOST_AUTO_UNIT_TEST(executor)
{
    senf::console::root().mkdir("dir1").mkdir("dir3");
    senf::console::root().mkdir("dir2").doc("Helptext").add("test",&testCommand);

    senf::console::Executor executor;
    senf::console::CommandParser parser;

    BOOST_CHECK( &executor.cwd() == &senf::console::root() );
    
    {
        std::stringstream os;
        parser.parse("cd dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir1"] );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("cd /dir2", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir2"] );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("cd dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir2"] );
        BOOST_CHECK_EQUAL( os.str(), "invalid directory\n" );
    }

    {
        std::stringstream os;
        parser.parse("cd /", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root() );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("ls", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLS );
        BOOST_CHECK_EQUAL( os.str(), "dir1/\ndir2/\n" );
    }

    {
        std::stringstream os;
        parser.parse("ls dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLS );
        BOOST_CHECK_EQUAL( os.str(), "dir3/\n" );
    }

    {
        std::stringstream os;
        parser.parse("ls dir3", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLS );
        BOOST_CHECK_EQUAL( os.str(), "invalid directory\n" );
    }
    
    {
        std::stringstream os;
        parser.parse("dir1/dir3 { }", &setCommand);
        executor(os, commands.rbegin()[1]);
        BOOST_CHECK_EQUAL( commands.rbegin()[1].builtin(), senf::console::ParseCommandInfo::BuiltinPUSHD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir1"]["dir3"] );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinPOPD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root() );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("exit", &setCommand);
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinEXIT );
        BOOST_CHECK_THROW( executor(os, commands.back()), senf::console::Executor::ExitException );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("help /dir2", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinHELP );
        BOOST_CHECK_EQUAL( os.str(), "senf::console::DirectoryNode at /dir2\n\nHelptext\n" );
    }

    {
        std::stringstream os;
        parser.parse("dir2/test", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::NoBuiltin );
        BOOST_CHECK_EQUAL( os.str(), "testCommand\n" );
    }

    commands.clear();
    senf::console::root().remove("dir1");
    senf::console::root().remove("dir2");
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
