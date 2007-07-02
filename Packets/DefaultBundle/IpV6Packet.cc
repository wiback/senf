// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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

/** \file
    \brief IpV6Packet non-inline non-template implementation */

#include "IpV6Packet.hh"
//#include "IpV6Packet.ih"

// Custom includes
#include "EthernetPacket.hh"
#include "Socket/INetAddressing.hh"

//#include "IpV6Packet.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::IpV6Packet>
        registerIpV6Packet (0x86dd);

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IpV6Packet>
        registerIpV6Packet2 (41); // IP6-in-IP(6) encapsulation

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::DataPacket>
        registerNoNextHeader (59);
}

prefix_ void senf::IpV6Packet::v_nextInterpreter()
    const
{
    registerInterpreter(nextHeader(),begin()+bytes(),end());
}

prefix_ void senf::IpV6Packet::v_finalize()
{}

prefix_ void senf::IpV6Packet::v_dump(std::ostream & os)
    const
{
    os << "Internet protocol Version 6:\n"
       << "  version       : " << unsigned(version()) << "\n"
       << "  traffic class : " << std::hex << unsigned(trafficClass()) << "\n"
       << "  flow label    : " << std::hex << unsigned(flowLabel()) << "\n"
       << "  length        : " << std::dec << unsigned(length()) << "\n"
       << "  next header   : " << unsigned(nextHeader()) << "\n"
       << "  hop limit     : " << unsigned(hopLimit()) << "\n"
       << "  source        : " << INet6Address(source().range()) << "\n"
       << "  destination   : " << INet6Address(destination().range()) << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IpV6Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
