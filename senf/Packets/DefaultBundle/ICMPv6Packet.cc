// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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
    \brief ICMPv6Packet non-inline non-template implementation */

#include "ICMPv6Packet.hh"
//#include "ICMPv6Packet.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>
#include <senf/Utils/IpChecksum.hh>
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 58, senf::ICMPv6Packet);

prefix_ boost::uint16_t senf::ICMPv6PacketParser::calcChecksum() 
    const
{
    senf::IPv6Packet ipv6 (packet().rfind<senf::IPv6Packet>(senf::nothrow));
    if (! ipv6) return 0u;
    
    senf::IpChecksum summer;

    ////////////////////////////////////////
    // IPv6 pseudo header
    summer.feed( ipv6->source().i(), 
                 ipv6->source().i() + senf::IPv6Packet::Parser::source_t::fixed_bytes );
    // need support for HopByHop routing header -> the destination used here must be the *final*
    // destination ...
    summer.feed( ipv6->destination().i(), 
                 ipv6->destination().i() + senf::IPv6PacketParser::destination_t::fixed_bytes );
    // packet length
    boost::uint32_t size (data().size());
    summer.feed((size>>24)&0xff);
    summer.feed((size>>16)&0xff);
    summer.feed((size>> 8)&0xff);
    summer.feed((size    )&0xff);
    // protocol number
    // summer.feed( 0u );
    // summer.feed( 0u );
    summer.feed( 0u );
    summer.feed( 58u );

    ////////////////////////////////////////
    // ICMP Packet
    summer.feed( i(), i()+checksum_offset );
    // checksum
    // summer.feed(0); summer.feed(0);
    summer.feed( i()+checksum_offset+2, data().end() );

    boost::uint16_t rv (summer.sum());
    return rv ? rv : 0xffffu;
}

prefix_ void senf::ICMPv6PacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "ICMPv6 protocol:\n"
       <<     "  type                    : " << unsigned(p->type()) <<"\n"
       <<     "  code                    : " << unsigned(p->code()) <<"\n"
       <<     "  checksum                : 0x" 
       << std::hex << std::setw(4) << unsigned(p->checksum()) << "\n";
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
