// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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

// Definition of non-inline non-template functions

//#include "server.hh"
//#include "server.ih"

// Custom includes
#include <string>
#include <iostream>
#include "Scheduler/Scheduler.hh"
#include "Utils/membind.hh"
#include "Socket/Protocols/INet/TCPSocketHandle.hh"
#include "Socket/Protocols/INet/INetAddressing.hh"


class Server
{
    senf::TCPv4ServerSocketHandle serverSock;

public:
    Server(std::string const & host, unsigned int port)
        : serverSock(senf::INet4SocketAddress(host, port)) {}
    
    void run() 
    {
        senf::Scheduler::instance().add(
            serverSock, 
            senf::membind(&Server::accept, this),
            senf::Scheduler::EV_READ);
        senf::Scheduler::instance().process();
    }
         
private:
    void accept(senf::FileHandle /* ignored */, senf::Scheduler::EventId event)
    {
        senf::TCPv4ClientSocketHandle clientSock (serverSock.accept());
        senf::Scheduler::instance().add(
            clientSock,
            senf::membind(&Server::readFromClient, this),
            senf::Scheduler::EV_READ);
    }
    
    void readFromClient(senf::TCPv4ClientSocketHandle clientSock, senf::Scheduler::EventId event)
    {
        if (!clientSock) {
            senf::Scheduler::instance().remove(clientSock);
            return;
        }
        std::string data (clientSock.read());
        std::cout << "'" << data << "'" << std::endl;
    }
};


int main(int argc, char const * argv[])
{
    try {
        Server myServer ("127.0.0.1", 4243);
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
