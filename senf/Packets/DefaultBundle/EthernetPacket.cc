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
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/Format.hh>
#include "LlcSnapPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::EthVLanPacketType::etherType, senf::EthVLanPacket);
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::EthOUIExtensionPacketType::etherType, senf::EthOUIExtensionPacket);

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
       << senf::fieldName("destination")               << p->destination() << std::endl
       << senf::fieldName("source")                    << p->source() << std::endl
       << senf::fieldName("type/length")               << format::dumpint(p->type_length().value()) << std::endl;
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthVLanPacketType

prefix_ void senf::EthVLanPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet 802.1q (VLAN):\n"
       << senf::fieldName("priority")                  << p->priority() << std::endl
       << senf::fieldName("cfi")                       << p->cfi() << std::endl
       << senf::fieldName("vlan-ID")                   << p->vlanId() << std::endl
       << senf::fieldName("ethertype")
       << " 0x" << std::hex << std::setw(4) << std::setfill('0') << std::right << p->type() << std::endl;
}

prefix_ void senf::EthVLanPacketType::finalize(packet p)
{
    p->type() << key(p.next(nothrow));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthVLanPacketType

prefix_ void senf::EthOUIExtensionPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet OUI Extension:\n"
       << std::hex << std::setfill('0') << std::right
       << senf::fieldName("OUI")            << std::setw(3) << " 0x" << p->oui() << std::endl
       << senf::fieldName("Ext. EtherType") << std::setw(2) << " 0x" << p->ext_type() << std::endl;
}

prefix_ void senf::EthOUIExtensionPacketType::finalize(packet p)
{
    optional_key_t k (key(p.next(nothrow)));
    if (k) {
        p->oui() << EtherOUIExtTypes::oui(*k);
        p->ext_type() << EtherOUIExtTypes::ext_type(*k);
    }
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
