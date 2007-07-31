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

/** \file
    \brief IpV4Packet non-inline non-template implementation */

#include "IpV4Packet.hh"
//#include "IpV4Packet.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EthernetPacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::IpV4Packet>
        registerIpV4Packet (0x0800);

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IpV4Packet>
        regsiterIpV4Packet2 (4); // IP-in-IP encapsulation
}

prefix_ void senf::IpV4PacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 4:\n"
       << "  version       : " << p->version() << "\n"
       << "  IHL           : " << p->ihl() << "\n"
       << "  TOS           : " << unsigned(p->tos()) << "\n"
       << "  length        : " << p->length() << "\n"
       << "  identifier    : " << p->identifier() << "\n"
       << "  DF            : " << p->df() << "\n"
       << "  MF            : " << p->mf() << "\n"
       << "  fragment      : " << p->frag() << "\n"
       << "  TTL           : " << unsigned(p->ttl()) << "\n"
       << "  protocol      : " << unsigned(p->protocol()) << "\n"
       << "  CRC           : " << std::hex << p->crc() << std::dec << "\n"
       << "  source        : " << p->source() << "\n"
       << "  destination   : " << p->destination() << "\n";
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
