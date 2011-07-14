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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

    senf::console::root().add("dir 1",fty::Directory()).add("dir3",fty::Directory());
    senf::console::root().add("dir2",fty::Directory()).doc("Helptext").add("test",fty::Command(&testCommand));

    senf::console::Executor executor;
    senf::console::CommandParser parser;

    BOOST_CHECK( &executor.cwd() == &senf::console::root() );

    {
        std::stringstream os;
        parser.parse("cd \"dir 1\"", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinCD );
        BOOST_CHECK( executor.cwd() == senf::console::root()["dir 1"] );
        BOOST_CHECK_EQUAL( executor.cwdPath(), "/dir 1" );
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
        parser.parse("cd \"dir 1\"", &setCommand);
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
                           "dir 1/                      \n"
                           "dir2/                       Helptext\n"
                           "sys/                        \n" );
    }

    {
        std::stringstream os;
        parser.parse("ll \"dir 1\"", &setCommand);
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
                           "dir 1/                                  \n"
                           "  dir3/                                 \n"
                           "dir2/                                   Helptext\n"
                           "  test                                  \n"
                           "sys/                                    \n" );
    }

    {
        std::stringstream os;
        parser.parse("\"dir 1\"/dir3 { }", &setCommand);
        executor(os, commands.rbegin()[1]);
        BOOST_CHECK_EQUAL( commands.rbegin()[1].builtin(), senf::console::ParseCommandInfo::BuiltinPUSHD );
        BOOST_CHECK( &executor.cwd() == &senf::console::root()["dir 1"]["dir3"] );
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
    senf::console::root().remove("dir 1");
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
