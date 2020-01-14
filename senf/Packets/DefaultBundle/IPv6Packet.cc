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
    \brief IPv6Packet non-inline non-template implementation */

#include "IPv6Packet.hh"
//#include "IPv6Packet.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "EthernetPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::IPv6PacketType::etherType, senf::IPv6Packet);
SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes,    41,                              senf::IPv6Packet); // IP6-in-IP(6) encapsulation

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::IPv6Packet );

prefix_ void senf::IPv6PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Internet protocol Version 6:\n"
       << senf::fieldName("version")                   << unsigned(p->version()) << "\n"
       << senf::fieldName("traffic class")
       << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::right << unsigned(p->trafficClass()) << "\n"
       << senf::fieldName("flow label")
       << "0x" << std::hex << std::setw(5) << std::setfill('0') << std::right << unsigned(p->flowLabel()) << "\n"
       << senf::fieldName("payload length")            << std::dec << unsigned(p->length()) << "\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("hop limit")                 << unsigned(p->hopLimit()) << "\n"
       << senf::fieldName("source")                    << p->source() << "\n"
       << senf::fieldName("destination")               << p->destination() << "\n";
}

prefix_ void senf::IPv6PacketType::finalize(packet p)
{
    p->length() << (p.size() - IPv6PacketParser::fixed_bytes);
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "IPv6Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
