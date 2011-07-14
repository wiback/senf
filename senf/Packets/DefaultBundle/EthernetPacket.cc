// $Id$
//
// Copyright (C) 2006
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief EthernetPacket non-inline non-template implementation */

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include "LlcSnapPacket.hh"
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/Format.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, 0x8100, senf::EthVLanPacket);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthernetPacketType

prefix_ void senf::EthernetPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    if (p->type_length() <= 1500)
        os << "Ethernet 802.3";
    else if (p->type_length() >= 0x600)
        os << "Ethernet II (DIX)";
    else
        os << "Ethernet 802.3 (bad ethertype >1500 and <1536)";
    os <<     ": \n"
       << senf::fieldName("destination")               << p->destination() << "\n"
       << senf::fieldName("source")                    << p->source() << "\n"
       << senf::fieldName("type/length")               << senf::format::dumpint(p->type_length().value()) << "\n";
}

prefix_ senf::PacketInterpreterBase::factory_t senf::EthernetPacketType::nextPacketType(packet p)
{
    if      (p->type_length() >= 1536) return lookup(p->type_length());
    else if (p->type_length() <= 1500) return LlcSnapPacket::factory();
    else                               return no_factory();
}

prefix_ void senf::EthernetPacketType::finalize(packet p)
{
    Packet n (p.next(nothrow));
    if (n) {
        optional_key_t k (key(n));
        if (k)
            p->type_length() << k;
        else if (n.is<LlcSnapPacket>())
            p->type_length() << n.data().size();
    }
    // Do NOT reset type_length if the type is not known ... doing this will destroy read packets
}

prefix_ void senf::EthVLanPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet 802.1q (VLAN):\n"
       << senf::fieldName("priority")                  << p->priority() << "\n"
       << senf::fieldName("cfi")                       << p->cfi() << "\n"
       << senf::fieldName("vlan-ID")                   << p->vlanId() << "\n"
       << senf::fieldName("ethertype")
       << " 0x" << std::hex << std::setw(4) << std::setfill('0') << std::right << p->type() << "\n";
}

prefix_ void senf::EthVLanPacketType::finalize(packet p)
{
    p->type() << key(p.next(nothrow));
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
