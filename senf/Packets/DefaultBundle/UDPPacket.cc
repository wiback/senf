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
    \brief UDPPacket non-inline non-template implementation */

#include "UDPPacket.hh"
//#include "UDPPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "../../Packets/Packets.hh"
#include "../../Utils/IpChecksum.hh"
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::UDPPacket>
        registerUDPPacket (17);
}

///////////////////////////////////////////////////////////////////////////
// senf::UDPPacketParser

prefix_ boost::uint16_t senf::UDPPacketParser::calcChecksum()
    const
{
    IpChecksum summer;
    // first on to the awkward part: the IP pseudo header
    IPv4Packet ipv4 (packet().rfind<IPv4Packet>(nothrow));
    if (ipv4) {
        // Pseudo header defined in RFC768
        summer.feed( ipv4->source().i(), 
                     ipv4->source().i() + IPv4Packet::Parser::source_t::fixed_bytes );
        ///\fixme What about a hop-by-hop routing option? Which destination is used in IPv4 ?
        summer.feed( ipv4->destination().i(), 
                     ipv4->destination().i() + IPv4PacketParser::destination_t::fixed_bytes );
        summer.feed( 0u );
        ///\fixme May there be another header between the IPv4 header and UDP? if so, we
        /// need to hack the correct protocol number here ...
        summer.feed( 17u );
        summer.feed( i() + length_offset, i() + length_offset + 2 );
    } 
    else {
        // Pseudo header defined in RFC2460
        IPv6Packet ipv6 (packet().rfind<IPv6Packet>(nothrow));
        if (ipv6) {
            summer.feed( ipv6->source().i(), 
                         ipv6->source().i() + IPv6Packet::Parser::source_t::fixed_bytes );
            ///\todo Implement routing header support
            // The destination used here must be the *final* destination ...
            summer.feed( ipv6->destination().i(), 
                         ipv6->destination().i() + IPv6PacketParser::destination_t::fixed_bytes );
            // This is a simplification. The value is really 32bit to support UDP Jumbograms
            // (RFC2147). However, skipping an even number of 0 bytes does not change the checksum
            summer.feed( i() + length_offset, i() + length_offset + 2 );
            // RFC2460 specifies, that this must always be 17, not the value used in the ipv6
            // header
            summer.feed( 0u );
            summer.feed( 17u );
        }
    }
    
    // since header are 16 / even 32bit aligned we don't have to care for padding. since IpChecksum 
    // cares for padding at the final summing we don't have to care is the payload is 16nbit-aligned, too.
    summer.feed( i(), i()+checksum_offset );
    summer.feed( i()+checksum_offset+2, data().end() );

    boost::uint16_t rv (summer.sum());
    return rv ? rv : 0xffffu;
}

///////////////////////////////////////////////////////////////////////////
// senf::UDPPacketType

prefix_ void senf::UDPPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "UDP:\n"
       <<     "  source port             : " << p->source() << "\n"
       <<     "  dest port               : " << p->destination() << "\n"
       <<     "  length                  : " << p->length() << "\n"
       <<     "  checksum                : " 
       << std::hex << std::setw(4) << std::setfill('0') << p->checksum() << "\n";
}

prefix_ void senf::UDPPacketType::finalize(packet p)
{
    p->length() << p.size();
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
