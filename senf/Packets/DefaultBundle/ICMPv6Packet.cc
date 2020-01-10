//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::ICMPv6PacketType::ipType, senf::ICMPv6Packet);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6Packet );

prefix_ boost::uint16_t senf::ICMPv6PacketParser::calcChecksum()
    const
{
    IPv6Packet ipv6 (packet().rfind<IPv6Packet>(senf::nothrow));
    if (! ipv6) return 0u;

    IpChecksum summer;

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // IPv6 pseudo header
    summer.feed( ipv6->source().i(),
                 ipv6->source().i() + IPv6Packet::Parser::source_t::fixed_bytes );
    // need support for HopByHop routing header -> the destination used here must be the *final*
    // destination ...
    summer.feed( ipv6->destination().i(),
                 ipv6->destination().i() + IPv6PacketParser::destination_t::fixed_bytes );
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

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // ICMP Packet
    summer.feed( i(), i()+checksum_offset );
    // checksum
    // summer.feed(0); summer.feed(0);
    summer.feed( i()+checksum_offset+2, data().end() );

    boost::uint16_t rv (summer.sum());
    return rv ? rv : 0xffffu;
}

prefix_ void senf::ICMPv6PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "ICMPv6 protocol:\n"
       << senf::fieldName("type") << unsigned(p->type()) << "\n"
       << senf::fieldName("code") << unsigned(p->code()) << "\n"
       << senf::fieldName("checksum")
       << "0x" << std::hex << std::setw(4) << unsigned(p->checksum()) << "\n";
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
