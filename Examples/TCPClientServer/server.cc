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
