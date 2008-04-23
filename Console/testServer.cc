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
#include <senf/Console.hh>
#include <senf/Scheduler/Scheduler.hh>

namespace kw = senf::console::kw;

void echo(std::ostream & output, senf::console::ParseCommandInfo const & command)
{
    typedef senf::console::ParseCommandInfo::ArgumentsRange::iterator iterator;
    iterator i (command.arguments().begin());
    iterator i_end (command.arguments().end());
    for (; i != i_end; ++i) {
        iterator::value_type::iterator j (i->begin());
        iterator::value_type::iterator j_end (i->end());
        for (; j != j_end; ++j) 
            output << j->value() << ' ';
    }
    output << "\n";
}

struct TestObject
{
    senf::console::ScopedDirectory<TestObject> dir;

    TestObject() 
        : dir(this) 
        {
            dir.add("vat", &TestObject::vat)
                .arg("vat", "VAT in %", kw::default_value = 19)
                .arg("amount", "Amount including VAT")
                .doc("Returns the amount of {vat}-% VAT included in {amount}");
        }

    double vat (int vat, double amount) 
        {
            return amount * vat/(100.0+vat);
        }
};

void shutdownServer()
{
    senf::Scheduler::instance().terminate();
    throw senf::console::Executor::ExitException();
}

int main(int, char **)
{
    senf::log::ConsoleTarget::instance().route< senf::SenfLog, senf::log::NOTICE >();

    senf::console::root()
        .doc("This is the console test application");
    senf::console::root()
        .mkdir("network")
        .doc("Network related settings");
    senf::console::root()["network"]
        .mkdir("eth0")
        .doc("Ethernet device eth0");
    senf::console::root()
        .mkdir("server");
    senf::console::root()["server"]
        .add("shutdown", &shutdownServer)
        .doc("Terminate server application");
    senf::console::root()["network"]
        .add("echo", &echo)
        .doc("Example of a function utilizing manual argument parsing");

    TestObject test;
    senf::console::root()
        .add("testob", test.dir)
        .doc("Example of an instance directory");

    senf::console::Server::start( senf::INet4SocketAddress(23232) )
        .name("testServer");

    senf::Scheduler::instance().process();
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
