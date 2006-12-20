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
#include "Socket/PacketSocketHandle.hh"
#include "Packets/EthernetPacket.hh"
#include "Packets/IpV4Packet.hh"
#include "Packets/UDPPacket.hh"

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

    void use_em()
    {
        // Pull in symbols from the wanted packets ...
        senf::Packet::create<senf::IpV4Packet>(0,0);
        senf::Packet::create<senf::UDPPacket>(0,0);
    }
}

int main (int argc, char const * argv[])
{
    try {
        senf::PacketSocketHandle sock;
        sock.bind(senf::LLSocketAddress("eth0"));
        // sock.protocol().promisc("eth0",senf::PacketProtocol::Promiscuous);
        
        while (true) { // forever
            std::string data (sock.read());
            senf::EthernetPacket::ptr packet (
                senf::Packet::create<senf::EthernetPacket>(
                    data.begin(), data.end()));
            packet->dump(std::cout);
            hexdump(packet->last()->begin(),
                    packet->last()->end());
            std::cout << "\n\n";
        }
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Sniffer.mpp"


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
