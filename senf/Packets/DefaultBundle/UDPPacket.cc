//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief UDPPacket non-inline non-template implementation */

#include "UDPPacket.hh"
//#include "UDPPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/IpChecksum.hh>
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::UDPPacketType::ipType, senf::UDPPacket);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::UDPPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::UDPPacketType

prefix_ void senf::UDPPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "UDP:\n"
       << senf::fieldName("source port")               << p->source() << "\n"
       << senf::fieldName("dest port")                 << p->destination() << "\n"
       << senf::fieldName("length")                    << p->length() << "\n"
       << senf::fieldName("checksum")
       << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::right << p->checksum() << "\n";
}

prefix_ void senf::UDPPacketType::finalize(packet p)
{
    p->length() << p.size();
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
