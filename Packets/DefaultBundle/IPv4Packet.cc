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

/** \file
    \brief IPv4Packet non-inline non-template implementation */

#include "IPv4Packet.hh"
//#include "IPv4Packet.ih"

// Custom includes
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/io/ios_state.hpp>
#include "../../Utils/IpChecksum.hh"
#include "EthernetPacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::IPv4Packet>
        registerIPv4Packet (0x0800);

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv4Packet>
        regsiterIPv4Packet2 (4); // IP-in-IP encapsulation
}

///////////////////////////////////////////////////////////////////////////
// senf::IPv4PacketParser

prefix_ boost::uint16_t senf::IPv4PacketParser::calcChecksum()
    const
{
    validate(bytes(*this));
    IpChecksum summer;
    summer.feed( i(),                   i()+checksum_offset );
    // Not needed since the number of 0-bytes is even
    // summer.feed( 0u );
    // summer.feed( 0u );
    summer.feed( i()+checksum_offset+2, i()+bytes(*this)    );
    return summer.sum();
}

///////////////////////////////////////////////////////////////////////////
// senf::IPv4PacketType

prefix_ void senf::IPv4PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
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
       << "  checksum      : 0x" 
         << std::hex << std::setw(4) << std::setfill('0') << p->checksum() << std::dec << "\n"
       << "  source        : " << p->source() << "\n"
       << "  destination   : " << p->destination() << "\n";
}

prefix_ void senf::IPv4PacketType::finalize(packet p)
{
    p->length()   << p.size();
    p->protocol() << key(p.next(nothrow));
    p->checksum() << p->calcChecksum();
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
