//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include <string>
#include <iostream>
#include <iomanip>
#include <senf/Socket/Protocols/Raw.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Scheduler/Scheduler.hh>

//#include "Sniffer.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

int loop_main (int argc, char const * argv[])
{
    try {
        senf::PacketSocketHandle sock;
        sock.bind(senf::LLSocketAddress(argv[2]));
        // sock.protocol().promisc("eth0",senf::PacketProtocol::Promiscuous);

        while (true) { // forever
            senf::EthernetPacket packet (senf::EthernetPacket::create(
                                             senf::noinit));
            sock.read(packet.data(),0);
            packet.dump(std::cout);
            senf::hexdump(
                    packet.last().data().begin(),
                    packet.last().data().end(),
                    std::cout);
            std::cout << "\n\n";
        }
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}

class Sniffer
{
    senf::PacketSocketHandle sock;
    senf::scheduler::FdEvent event;

public:
    Sniffer(std::string const & interface)
        : event ("Sniffer", senf::membind(&Sniffer::dumpPacket, this),
                 sock, senf::scheduler::FdEvent::EV_READ)
    {
        sock.bind(senf::LLSocketAddress(interface));
    }

    void run()
    {
        senf::scheduler::process();
    }

private:
    void dumpPacket(int event)
    {
        senf::EthernetPacket packet (
            senf::EthernetPacket::create(senf::noinit));
        sock.read(packet.data(),0);
        packet.dump(std::cout);
        senf::hexdump(
                packet.last().data().begin(),
                packet.last().data().end(),
                std::cout);
        std::cout << "\n\n";
    }
};

int scheduler_main(int argc, char const * argv[])
{
    try {
        Sniffer sniffer (argv[2]);
        sniffer.run();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}

int main(int argc, char const * argv[])
{
    std::cout << "Registered packets:\n\n";
    senf::dumpPacketRegistries(std::cout);

    if (argc >= 3) {
        if (std::string(argv[1]) == "loop")
            return loop_main(argc,argv);
        else if (std::string(argv[1]) == "scheduler")
            return scheduler_main(argc,argv);
    }

    std::cerr << "Usage: sniffer { loop | scheduler } [interface]" << std::endl;
    return 1;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Sniffer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
