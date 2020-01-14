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
    \brief EthernetPacket non-inline non-template implementation */

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/Format.hh>
#include <senf/Utils/senflikely.hh>
#include "LlcSnapPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, 0x6558,                                     senf::EthernetPacket       );
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::EthVLanSPacketType::etherType,        senf::EthVLanSPacket       );
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::EthVLanCPacketType::etherType,        senf::EthVLanCPacket       );
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::EthOUIExtensionPacketType::etherType, senf::EthOUIExtensionPacket);

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::EthernetPacket        );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::EthVLanSPacket         );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::EthVLanCPacket         );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::EthOUIExtensionPacket );

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
       << senf::fieldName("destination") << p->destination() << std::endl
       << senf::fieldName("source")      << p->source() << std::endl
       << senf::fieldName("type/length") << format::dumpint(p->type_length().value()) << std::endl;
}

prefix_ senf::PacketInterpreterBase::factory_t senf::EthernetPacketType::nextPacketType(packet p)
{
    if      (SENF_LIKELY(p->type_length() >= 1536)) return lookup(p->type_length());
    else if (p->type_length() <= 1500)              return LlcSnapPacket::factory();
    else                                            return no_factory();
}

prefix_ void senf::EthernetPacketType::finalize(packet p)
{
    Packet n (p.next(nothrow));
    if (SENF_LIKELY(n)) {
        optional_key_t k (key(n));
        if (SENF_LIKELY(k))
            p->type_length() << k;
        else if (n.is<LlcSnapPacket>())
            p->type_length() << n.data().size();
    }
    // Do NOT reset type_length if the type is not known ... doing this will destroy read packets
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthVLanSPacketType

prefix_ void senf::EthVLanSPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet 802.1ad (VLAN S-Tag):\n"
       << senf::fieldName("priority")    << p->priority() << std::endl
       << senf::fieldName("cfi")         << p->cfi() << std::endl
       << senf::fieldName("vlanId")     << p->vlanId() << std::endl
       << senf::fieldName("etherType")
            << "0x" << std::hex << std::setw(4) << std::setfill('0')
            << std::right << p->type_length() << std::endl;
}

prefix_ void senf::EthVLanSPacketType::finalize(packet p)
{
    p->type_length() << key(p.next(nothrow));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthVLanCPacketType

prefix_ void senf::EthVLanCPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet 802.1q (VLAN C-Tag):\n"
       << senf::fieldName("priority")    << p->priority() << std::endl
       << senf::fieldName("cfi")         << p->cfi() << std::endl
       << senf::fieldName("vlanId")     << p->vlanId() << std::endl
       << senf::fieldName("etherType")
            << "0x" << std::hex << std::setw(4) << std::setfill('0')
            << std::right << p->type_length() << std::endl;
}

prefix_ void senf::EthVLanCPacketType::finalize(packet p)
{
    p->type_length() << key(p.next(nothrow));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthOUIExtensionPacketType

prefix_ void senf::EthOUIExtensionPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet OUI Extension:\n"
       << std::hex << std::setfill('0') << std::right
       << senf::fieldName("OUI")            << std::setw(3) << "0x" << p->oui() << std::endl
       << senf::fieldName("Ext. EtherType") << std::setw(2) << "0x" << p->ext_type() << std::endl;
}

prefix_ void senf::EthOUIExtensionPacketType::finalize(packet p)
{
    optional_key_t k (key(p.next(nothrow)));
    if (SENF_LIKELY(k)) {
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
