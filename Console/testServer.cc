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
    \brief test non-inline non-template implementation */

//#include "test.hh"
//#include "test.ih"

// Custom includes
#include <iostream>
#include "Server.hh"
#include "Node.hh"
#include "../Scheduler/Scheduler.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "test.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct MyCommand : public senf::console::CommandNode
    {
        MyCommand(std::string name) : senf::console::CommandNode(name) {}
        void operator()(std::ostream & output, 
                        senf::console::ParseCommandInfo::ArgumentsRange const & arguments) {
            senf::console::ParseCommandInfo::argument_iterator i (arguments.begin());
            senf::console::ParseCommandInfo::argument_iterator i_end (arguments.end());
            for (; i != i_end; ++i) {
                senf::console::ParseCommandInfo::token_iterator j (i->begin());
                senf::console::ParseCommandInfo::token_iterator j_end (i->end());
                for (; j != j_end; ++j) 
                    output << j->value() << ' ';
            }
            output << "\n";
        }
    };
}

int main(int, char const **)
{
    senf::log::ConsoleTarget::instance().route< senf::SenfLog, senf::log::NOTICE >();

    senf::console::root().mkdir("network").mkdir("eth0");
    senf::console::root().mkdir("server");

    senf::console::root()["network"].add(
        std::auto_ptr<senf::console::GenericNode>(new MyCommand("route")));

    senf::console::Server::start( senf::INet4SocketAddress("127.0.0.1:23232") )
        .name("testServer");

    senf::Scheduler::instance().process();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "test.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U testServer"
// End:
