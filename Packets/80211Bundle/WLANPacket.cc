// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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

// Custom includes
#include "WLANPacket.hh"
#include "../../Packets/Packets.hh"
#include "../DefaultBundle/LlcSnapPacket.hh"
#include <boost/io/ios_state.hpp>

#define prefix_

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::destinationAddress()
    const
{
    switch (dsBits()) {
    case 0 :
    case 2 :
        return addr1();
    }
    return addr3();
}

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::sourceAddress()
    const
{
    switch (dsBits())
    {
    case 0 :
    case 1 :
        return addr2();
        break;
    case 2 :
        return addr3();
        break;
//TODO wds frames
//    case 3:
//        return addr4();
    }
    //just to avoid compiler warning
    //TODO
    return addr1();
}

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::bssid()
    const
{
    switch (dsBits())
    {
    case 0 :
        return addr3();
        break;
    case 1 :
        return addr1();
        break;
    case 2:
        return addr2();
    }
    //just to avoid compiler warning
    //TODO
    return addr1();
}

//shift some bits to read the 12bit sequence number bit field in LSB byte order
prefix_ boost::uint16_t senf::WLANPacket_MgtFrameParser::sequenceNumber()
    const
{
    boost::uint16_t seqN = 0;
    seqN |= seqNumber_2();
    seqN <<= 4;
    seqN |= seqNumber_1();
    return seqN;
}

//shift some bits to read the 12bit sequence number bit field in LSB byte order
prefix_ boost::uint16_t senf::WLANPacket_DataFrameParser::sequenceNumber()
    const
{
    boost::uint16_t seqN = 0;
    seqN |= seqNumber_2();
    seqN <<= 4;
    seqN |= seqNumber_1();
    return seqN;
}

prefix_ void senf::WLANPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Frame:\n"
       << "  Type           : " << unsigned( p->type()) << "\n"
       << "  Subtype        : " << unsigned( p->subtype()) << "\n"
       << "  Retransmission : " << unsigned( p->retry()) << "\n"
       << "  Duration       : " << unsigned( p->duration()) << "\n";
    if (p->is_mgtFrame()) {
        os << "  Management-Frame:\n" 
           << "    BSSID               : " << p->mgtFrame().bssid() << "\n"
           << "    Destination Address : " << p->mgtFrame().destinationAddress() << "\n"
           << "    Source Address      : " << p->mgtFrame().sourceAddress() << "\n"
           << "    Sequence Number     : " << unsigned( p->mgtFrame().sequenceNumber()) << "\n"
           << "    Fragment Number     : " << unsigned( p->mgtFrame().fragmentNumber()) << "\n";
    }
    if (p->is_ctrlFrame()) {
        os << "  Control-Frame ";
        if (p->ctrlFrame().is_cts()) os << "(CTS):\n";
        if (p->ctrlFrame().is_ack()) os << "(ACK):\n";
        if (p->ctrlFrame().is_rts()) os << "(RTS):\n";
        os << "    Receiver Address : " << p->ctrlFrame().receiverAddress() << "\n";
        if (p->ctrlFrame().is_rts())
            os << "    Source Address : " << p->ctrlFrame().sourceAddress() << "\n";
    }
    if (p->is_dataFrame()) {
        os << "  Data-Frame:\n"
           << "    Sequence Number     : " << unsigned( p->mgtFrame().sequenceNumber()) << "\n"
           << "    Fragment Number     : " << unsigned( p->mgtFrame().fragmentNumber()) << "\n";
    }
};




#undef prefix_
