// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
#include "Socket/Protocols/Raw/PacketSocketHandle.hh"
#include "Scheduler/Scheduler.hh"
#include "Utils/membind.hh"
#include "Packets/DefaultBundle/EthernetPacket.hh"

//#include "Sniffer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    static const unsigned BLOCK_SIZE = 16;

    template <class Iterator>
    void hexdump(Iterator i, Iterator const & i_end)
    {
        unsigned offset (0);
        std::string ascii;
        for (; i != i_end; ++i, ++offset) {
            switch (offset % BLOCK_SIZE) {
            case 0:
                if (!ascii.empty()) {
                    std::cout << "  " << ascii << "\n";
                    ascii = "";
                }
                std::cout << "  "
                          << std::hex << std::setw(4) << std::setfill('0')
                          << offset << ' ';
                break;
            case BLOCK_SIZE/2:
                std::cout << " ";
                ascii += ' ';
                break;
            }
            std::cout << ' ' << std::hex << std::setw(2) << std::setfill('0')
                      << unsigned(*i);
            ascii += (*i >= ' ' && *i < 126) ? *i : '.';
        }
        if (!ascii.empty()) {
            for (; (offset % BLOCK_SIZE) != 0; ++offset) {
                if ((offset % BLOCK_SIZE) == BLOCK_SIZE/2)
                    std::cout << " ";
                std::cout << "   ";
            }
            std::cout << "  " << ascii << "\n";
        }
        std::cout << std::dec;
    }

}

int loop_main (int argc, char const * argv[])
{
    try {
        senf::PacketSocketHandle sock;
        sock.bind(senf::LLSocketAddress("eth1"));
        // sock.protocol().promisc("eth0",senf::PacketProtocol::Promiscuous);

        while (true) { // forever
            senf::EthernetPacket packet (senf::EthernetPacket::create(
                                             senf::EthernetPacket::noinit));
            sock.read(packet.data(),0);
            packet.dump(std::cout);
            hexdump(packet.last().data().begin(),
                    packet.last().data().end());
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

public:
    Sniffer(std::string const & interface) 
    {
        sock.bind(senf::LLSocketAddress(interface)); 
    }

    void run() 
    {
        senf::Scheduler::instance().add(
            sock, senf::membind(&Sniffer::dumpPacket, this));
        senf::Scheduler::instance().process();
    }
         
private:
    void dumpPacket(senf::FileHandle /* ignored */, senf::Scheduler::EventId event)
    {
        senf::EthernetPacket packet (
            senf::EthernetPacket::create(senf::EthernetPacket::noinit));
        sock.read(packet.data(),0);
        packet.dump(std::cout);
        hexdump(packet.last().data().begin(),
                packet.last().data().end());
        std::cout << "\n\n";
    }
};

int scheduler_main(int argc, char const * argv[])
{
    try {
        Sniffer sniffer ("eth1");
        sniffer.run();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}

int main(int argc, char const * argv[])
{
    if (argc >= 2)
        if (std::string(argv[1]) == "loop")
            return loop_main(argc,argv);
        else if (std::string(argv[1]) == "scheduler")
            return scheduler_main(argc,argv);

    std::cerr << "Usage: sniffer { loop | scheduler }" << std::endl;
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
