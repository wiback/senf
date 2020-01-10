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
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>

class Server
{
    senf::UDPv4ClientSocketHandle serverSock;
    senf::scheduler::FdEvent event;

public:
    Server(senf::INet4Address const & host, unsigned int port)
        : serverSock(senf::INet4SocketAddress(host, port)),
          event("UDPv4ClientServer", senf::membind(&Server::readFromClient, this),
                serverSock, senf::scheduler::FdEvent::EV_READ, false)
        {}

    void run()
    {
        event.enable();
        senf::scheduler::process();
    }

private:
    void readFromClient(int event)
    {
        std::string data (serverSock.read());
        std::cout << "> " << data<<std::endl ;
    }
};

int main(int argc, char const * argv[])
{
    try {
        Server testSock(senf::INet4Address::Loopback, 4243);
        testSock.run();
    }

    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// End:
