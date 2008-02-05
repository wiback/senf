// Custom includes
#include <string>
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>

class Server
{
    senf::UDPv4ClientSocketHandle serverSock;

public:
    Server(senf::INet4Address const & host, unsigned int port)
        : serverSock(senf::INet4SocketAddress(host, port)) {}

    void run()
    {
        senf::Scheduler::instance().add(
                serverSock,
                senf::membind(&Server::readFromClient, this),
                senf::Scheduler::EV_READ);
        senf::Scheduler::instance().process();
    }

private:
    void readFromClient(senf::Scheduler::EventId event)
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
