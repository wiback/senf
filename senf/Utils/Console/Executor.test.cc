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
    \brief Executor unit tests */

//#include "Executor.test.hh"
//#include "Executor.test.ih"

// Custom includes
#include <sstream>
#include <vector>
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
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

SENF_AUTO_UNIT_TEST(executor)
{
    namespace fty = senf::console::factory;

    senf::console::root().add("dir1",fty::Directory()).add("dir3",fty::Directory());
    senf::console::root().add("dir2",fty::Directory()).doc("Helptext").add("test",fty::Command(&testCommand));

    senf::console::Executor executor;
    senf::console::CommandParser parser;

    BOOST_CHECK( &executor.cwd() == &senf::console::root() );
    
    {
        std::stringstream os;
        parser.parse("cd dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( executor.cwd() == senf::console::root()["dir1"] );
        BOOST_CHECK_EQUAL( executor.cwdPath(), "/dir1" );
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
        BOOST_CHECK_THROW( executor(os, commands.back()), senf::console::SyntaxErrorException );
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir2"] );
        BOOST_CHECK_EQUAL( os.str(), "" );
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
        parser.parse("ll", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLL );
        BOOST_CHECK_EQUAL( os.str(), 
                           "dir1/                       \n"
                           "dir2/                       Helptext\n"
                           "sys/                        \n" );
    }

    {
        std::stringstream os;
        parser.parse("ll dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLL );
        BOOST_CHECK_EQUAL( os.str(), "dir3/                       \n" );
    }

    {
        std::stringstream os;
        parser.parse("ls dir3", &setCommand);
        BOOST_CHECK_THROW( executor(os, commands.back()), senf::console::SyntaxErrorException );
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLS );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }
    
    {
        std::stringstream os;
        parser.parse("lr", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLR );
        BOOST_CHECK_EQUAL( os.str().substr(0,213),
                           "dir1/                                   \n"
                           "  dir3/                                 \n"
                           "dir2/                                   Helptext\n"
                           "  test                                  \n"
                           "sys/                                    \n" );
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
        BOOST_CHECK_EQUAL( os.str(), "Helptext\n" );
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

SENF_AUTO_UNIT_TEST(executorChroot)
{
    namespace fty = senf::console::factory;

    senf::console::root().add("dir1",fty::Directory()).add("dir3",fty::Directory());
    senf::console::root().add("dir2",fty::Directory().doc("Helptext"))
        .add("test", fty::Command(&testCommand));

    senf::console::Executor executor;
    senf::console::CommandParser parser;

    executor.chroot( senf::console::root()["dir2"] );

    BOOST_CHECK( & executor.chroot() == & senf::console::root()["dir2"] );

    {
        std::stringstream os;
        parser.parse("../test", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::NoBuiltin );
        BOOST_CHECK_EQUAL( os.str(), "testCommand\n" );
    }

    commands.clear();
    senf::console::root().remove("dir1");
    senf::console::root().remove("dir2");
}

namespace {
    
    void testPolicy(senf::console::DirectoryNode & dir, std::string const & entry)
    {
        if (dir == senf::console::root() && entry == "dir2")
            throw senf::console::Executor::IgnoreCommandException();
    }
}

SENF_AUTO_UNIT_TEST(executorPolicy)
{
    namespace fty = senf::console::factory;

    senf::console::root().add("dir1",fty::Directory()).add("dir3",fty::Directory());
    senf::console::root().add("dir2",fty::Directory().doc("Helptext"))
        .add("test",fty::Command(&testCommand));

    senf::console::Executor executor;
    senf::console::CommandParser parser;

    executor.policy(&testPolicy);

    {
        std::stringstream os;
        parser.parse("ll dir1", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinLL );
        BOOST_CHECK_EQUAL( os.str(), "dir3/                       \n" );
    }

    {
        std::stringstream os;
        parser.parse("dir2/test", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::NoBuiltin );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    commands.clear();
    senf::console::root().remove("dir1");
    senf::console::root().remove("dir2");
}

SENF_AUTO_UNIT_TEST(executorAuto)
{
    namespace fty = senf::console::factory;

    senf::console::root().add("tdir1",fty::Directory()).add("dir3",fty::Directory());
    senf::console::root().add("dir2",fty::Directory().doc("Helptext"))
        .add("test",fty::Command(&testCommand));

    senf::console::Executor executor;
    executor
        .autocomplete(true)
        .autocd(true);
    senf::console::CommandParser parser;

    {
        std::stringstream os;
        parser.parse("dir2", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( executor.cwdPath(), "/dir2" );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("..", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( executor.cwdPath(), "/" );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }

    {
        std::stringstream os;
        parser.parse("d", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( executor.cwdPath(), "/dir2" );
        BOOST_CHECK_EQUAL( os.str(), "" );
    }
    
    {
        std::stringstream os;
        parser.parse("t", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( os.str(), "testCommand\n" );
    }
    
    commands.clear();
    senf::console::root().remove("tdir1");
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
