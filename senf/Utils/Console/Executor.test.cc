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
    \brief Executor unit tests */

// Custom includes
#include <sstream>
#include <vector>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(executor)
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
        parser.parse("echo test 1 2 3", &setCommand);
        executor(os, commands.back());
        BOOST_CHECK_EQUAL( commands.back().builtin(), senf::console::ParseCommandInfo::BuiltinECHO );
        BOOST_CHECK_EQUAL( os.str(), "test 1 2 3\n" );
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

SENF_AUTO_TEST_CASE(executorChroot)
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

SENF_AUTO_TEST_CASE(executorPolicy)
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

SENF_AUTO_TEST_CASE(executorAuto)
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
