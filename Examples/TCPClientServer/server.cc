// $Id$
//
// Copyright (C) 2007
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
//   Thorsten Horstmann <tho@berlios.de>


// Definition of non-inline non-template functions

//#include "server.hh"
//#include "server.ih"

// Custom includes
#include <string>
#include <iostream>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Socket/Protocols/INet.hh>

class Server
{
    senf::TCPv4ServerSocketHandle serverSock;
    senf::scheduler::FdEvent acceptevent;
    senf::scheduler::FdEvent readevent;

public:
    Server(senf::INet4Address const & host, unsigned int port)
        : serverSock(senf::INet4SocketAddress(host, port)),
          acceptevent("Server accept", senf::membind(&Server::accept, this),
                      serverSock, senf::scheduler::FdEvent::EV_READ),
          readevent("Server read", 0)
        {}

    void run()
    {
        senf::scheduler::process();
    }

private:
    void accept(int event)
    {
        senf::TCPv4ClientSocketHandle clientSock (serverSock.accept());
        readevent
            .action(boost::bind(&Server::readFromClient, this, clientSock, _1))
            .handle(clientSock)
            .events(senf::scheduler::FdEvent::EV_READ)
            .enable();
    }

    void readFromClient(senf::TCPv4ClientSocketHandle clientSock, int event)
    {
        if (!clientSock) {
            readevent.disable();
            return;
        }
        std::string data (clientSock.read());
        std::cout << "'" << data << "'" << std::endl;
    }
};


int main(int argc, char const * argv[])
{
    try {
        Server myServer (senf::INet4Address::Loopback, 4243);
        myServer.run();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
