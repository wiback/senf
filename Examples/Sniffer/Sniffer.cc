// $Id$
//
// Copyright (C) 2006
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

// Definition of non-inline non-template functions

//#include "Sniffer.hh"
//#include "Sniffer.ih"

// Custom includes
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
///////////////////////////////cc.p////////////////////////////////////////

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

///////////////////////////////cc.e////////////////////////////////////////
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
