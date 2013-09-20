// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):

/** \file
    \brief TCPPacket non-inline non-template implementation */

#include "TCPPacket.hh"
//#include "TCPPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/IpChecksum.hh>
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::TCPPacketType::ipType, senf::TCPPacket);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::TCPPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::TCPPacketParser

prefix_ boost::uint16_t senf::TCPPacketParser::calcChecksum()
    const
{
    IpChecksum summer;
    // first on to the awkward part: the IP pseudo header
    IPv4Packet ipv4 (packet().rfind<IPv4Packet>(nothrow));
    if (ipv4) {
        // Pseudo header defined in RFC793
        // begin
        summer.feed( ipv4->source().i(),
                     ipv4->source().i() + IPv4Packet::Parser::source_t::fixed_bytes );
        ///\fixme What about a hop-by-hop routing option? Which destination is used in IPv4 ?
        summer.feed( ipv4->destination().i(),
                     ipv4->destination().i() + IPv4PacketParser::destination_t::fixed_bytes );
        // include zero byte
        summer.feed( 0u );
        ///\fixme May there be another header between the IPv4 header and TCP? if so, we
        /// need to hack the correct protocol number here ...
        summer.feed( 6u );

        // feed 1st byte and 2nd byte from field tcp length
        summer.feed(boost::uint16_t(data().size()) & 0xff00);
        summer.feed(boost::uint16_t(data().size()) & 0x00ff);
        // end pseudo header
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
            // RFC2460 specifies, that this must always be 6, not the value used in the ipv6
            // header
            // feed 1st byte and 2nd byte from field tcp length
            summer.feed(boost::uint16_t(data().size()) & 0xff00);
            summer.feed(boost::uint16_t(data().size()) & 0x00ff);
            // include zero byte
            summer.feed( 0u );
            // add TCP protocol number
            summer.feed( 6u );
        }
    }

    // since header are 16 / even 32bit aligned we don't have to care for padding. since IpChecksum
    // cares for padding at the final summing we don't have to care is the payload is 16nbit-aligned, too.

    // iterate till checksum field and skip 2 bytes,
    // iterate over rest of tcp header and tcp payload
    summer.feed( i(), i()+16 );
    summer.feed( i()+18, data().end() );

    boost::uint16_t rv (summer.sum());
    return rv ? rv : 0xffffu;

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::TCPPacketType

prefix_ void senf::TCPPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "TCP:\n"
       << senf::fieldName("source port")               << p->source() << "\n"
       << senf::fieldName("destination port")          << p->destination() << "\n"
       << senf::fieldName("sequence number")           << p->sequencenumber() << "\n"
       << senf::fieldName("acknowledgment number")     << p->acknowledgmentnumber() << "\n"
       << senf::fieldName("data offset")               << p->dataoffset() << "\n"
       << senf::fieldName("urgent flag")               << p->urgf() << "\n"
       << senf::fieldName("ack flag")                  << p->ackf() << "\n"
       << senf::fieldName("push flag")                 << p->pshf() << "\n"
       << senf::fieldName("reset flag")                << p->rstf() << "\n"
       << senf::fieldName("syn flag")                  << p->synf() << "\n"
       << senf::fieldName("fin flag")                  << p->finf() << "\n"
       << senf::fieldName("window size")               << p->window() << "\n"
       << senf::fieldName("checksum")
       << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::right << p->checksum() << "\n"
       << senf::fieldName("urgent pointer")            << p->urgentpointer() << "\n";
}

prefix_ void senf::TCPPacketType::finalize(packet p)
{
    p->dataoffset() << p.size();
    p->checksum() << p->calcChecksum();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
