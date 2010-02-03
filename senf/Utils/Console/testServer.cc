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
    \brief testServer implementation */

// Custom includes
#include <iostream>
#include <senf/Utils/Console.hh>
#include <senf/Scheduler/Scheduler.hh>

namespace kw = senf::console::kw;
namespace fty = senf::console::factory;

void echo(std::ostream & output, senf::console::ParseCommandInfo const & command)
{
    typedef senf::console::ParseCommandInfo::TokensRange::iterator iterator;
    iterator i (command.tokens().begin());
    iterator i_end (command.tokens().end());
    for (; i != i_end; ++i) {
        output << i->value() << ' ';
    }
    output << "\n";
}

struct TestObject
{
    senf::console::ScopedDirectory<TestObject> dir;

    TestObject() 
        : dir(this) 
        {
            dir.add("vat", fty::Command(&TestObject::vat, this)
                    .arg("vat", "VAT in %", kw::default_value = 19)
                    .arg("amount", "Amount including VAT")
                    .doc("Returns the amount of {vat}-% VAT included in {amount}") );
        }

    double vat (int vat, double amount) 
        {
            return amount * vat/(100.0+vat);
        }
};

void shutdownServer()
{
    senf::scheduler::terminate();
    throw senf::console::Executor::ExitException();
}

void enableLogging(std::ostream & os)
{
    senf::console::Client::get(os).route<senf::log::NOTICE>();
}

int main(int, char **)
{
    ::signal(SIGPIPE, SIG_IGN);
    senf::log::ConsoleTarget::instance().route< senf::log::VERBOSE >();

    senf::console::root()
        .doc("This is the console test application");

    senf::console::root()
        .add("console",fty::Directory()
             .doc("Console settings"));

    senf::console::DirectoryNode & serverDir (
        senf::console::root()
        .add("server",fty::Directory()
             .doc("server commands")));

    senf::console::ScopedDirectory<> testDir;
    senf::console::root()
        .add("test", testDir)
        .doc("Test functions");

    senf::console::root()["console"]
        .add("showlog", fty::Command(&enableLogging)
             .doc("Enable display of log messages on the current console"));

    senf::console::root().add("sl", fty::Link(senf::console::root()["console"]("showlog")));

    serverDir
        .add("shutdown", fty::Command(&shutdownServer)
             .doc("Terminate server application"));

    testDir
        .add("echo", fty::Command(&echo)
             .doc("Example of a function utilizing manual argument parsing"));

    TestObject test;
    testDir
        .add("extra", test.dir)
        .doc("Example of an instance directory");

    senf::console::root().add("ex", fty::Link(test.dir));

    senf::console::Server::start( senf::INet4SocketAddress(23232u) )
        .name("testServer");

    senf::scheduler::process();
}


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U testServer"
// End:
