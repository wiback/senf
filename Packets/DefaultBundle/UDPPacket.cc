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

#include "UDPPacket.hh"
//#include "UDPPacket.ih"
#include "IpV4Packet.hh"

// Custom includes
#include "Packets/DataPacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::UDPPacket>
        registerUDPPacket (17);
}

prefix_ void senf::UDPPacket::v_nextInterpreter()
    const
{
    registerInterpreter<DataPacket>(begin()+bytes(),end());
}

prefix_ void senf::UDPPacket::v_finalize()
{}

prefix_ void senf::UDPPacket::v_dump(std::ostream & os)
    const
{
    os << "UDP:\n"
       << "  source port   : " << source() << "\n"
       << "  dest port     : " << destination() << "\n"
       << "  length        : " << length() << "\n"
       << "  crc           : " << std::hex << crc() << std::dec << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
