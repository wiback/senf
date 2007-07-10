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

//#include "MCSniffer.hh"
//#include "MCSniffer.ih"

// Custom includes
#include <fstream>
#include <string>
#include <iomanip>
#include "Socket/UDPSocketHandle.hh"
#include "Scheduler/Scheduler.hh"
#include "Packets/DefaultBundle/EthernetPacket.hh"
#include "Utils/membind.hh"


//#include "MCSniffer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    static const unsigned BLOCK_SIZE = 16;

    template <class Iterator>
    void hexdump(Iterator i, Iterator const & i_end, std::ostream& stream)
    {
        unsigned offset (0);
        std::string ascii;
        for (; i != i_end; ++i, ++offset) {
            switch (offset % BLOCK_SIZE) {
            case 0:
                if (!ascii.empty()) {
                    stream << "  " << ascii << "\n";
                    ascii = "";
                }
                stream << "  "
                          << std::hex << std::setw(4) << std::setfill('0')
                          << offset << ' ';
                break;
            case BLOCK_SIZE/2:
                stream << " ";
                ascii += ' ';
                break;
            }
            stream << ' ' << std::hex << std::setw(2) << std::setfill('0')
                      << unsigned(*i);
            ascii += (*i >= ' ' && *i < 126) ? *i : '.';
        }
        if (!ascii.empty()) {
            for (; (offset % BLOCK_SIZE) != 0; ++offset) {
                if ((offset % BLOCK_SIZE) == BLOCK_SIZE/2)
                    stream << " ";
                stream << "   ";
            }
            stream << "  " << ascii << "\n";
        }
        stream << std::dec;
    }
}


class MCSniffer
{
    senf::UDPv4ClientSocketHandle sock;
    std::ostream& stream;

public:
    MCSniffer(senf::INet4Address addr, std::ostream& s)
        : stream(s)
    {
        sock.protocol().bind(addr);
        sock.protocol().mcLoop(true);
        sock.protocol().mcAddMembership(addr);
        senf::Scheduler::instance().add(
            sock, senf::membind(&MCSniffer::dumpPacket, this));
    }

private:
    void dumpPacket(senf::FileHandle /* ignored */, senf::Scheduler::EventId event)
    {
        std::string data (sock.read());
        senf::EthernetPacket::ptr packet (
            senf::Packet::create<senf::EthernetPacket>(
                data.begin(), data.end()));
        packet->dump(stream);
        hexdump(packet->last()->begin(),
                packet->last()->end(),
                stream);
        stream << "\n\n";
     }
};


int main(int argc, char const * argv[])
{       
    try {
        std::ofstream f1 ("233.132.152.1.txt");
        std::ofstream f2 ("233.132.152.2.txt");
        
        MCSniffer sniffer1 (
            senf::INet4Address::INet4Address("233.132.152.1:22344"), f1);
        MCSniffer sniffer2 (
            senf::INet4Address::INet4Address("233.132.152.2:22344"), f2);
            
        senf::Scheduler::instance().process();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}

 
///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MCSniffer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
