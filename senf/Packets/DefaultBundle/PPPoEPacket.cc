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
    \brief PPPoEPacket non-inline non-template implementation */

#include "PPPoEPacket.hh"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "IPv4Packet.hh"
#include "IPv6Packet.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::PPPoEDPacketType::etherType, senf::PPPoEDPacket );  // PPPoE Discovery
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::PPPoESPacketType::etherType, senf::PPPoESPacket );  // PPPoE Session

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::PPPoEDPacket );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::PPPoESPacket );

template <typename PacketType>
prefix_ void senf::PPPoEPacketTypeBase<PacketType>::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "PPPoE:\n"
       << std::hex << std::setfill('0') << std::right
       << senf::fieldName("  version")     << "0x" << p->version() << "\n"
       << senf::fieldName("  type")        << "0x" << p->type() << "\n"
       << senf::fieldName("  code")        << "0x" << unsigned(p->code()) << "\n"
       << senf::fieldName("  sessionId")   << "0x" << p->sessionId() << "\n"
       << senf::fieldName("  length")      << "0x" << p->length() << "\n"
       << senf::fieldName("  pppProtocol") << "0x" << p->pppProtocol() << "\n";
}

template <typename PacketType>
prefix_ senf::PacketInterpreterBase::factory_t senf::PPPoEPacketTypeBase<PacketType>::nextPacketType(packet p)
{
    switch (p->pppProtocol()) {
    case 0x0021:
        if (p->code() == 0x0)
            return IPv4Packet::factory();
        break;
    case 0x0057:
        if (p->code() == 0x0)
            return IPv6Packet::factory();
        break;
    default:
        // there are other types, such as PAP, CHAP, LCP, etc....
        break;
    }
    return no_factory();
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
