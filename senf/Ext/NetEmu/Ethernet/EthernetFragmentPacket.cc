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


#include "EthernetFragmentPacket.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// wiback::EthernetFragmentPacket

SENF_PACKET_REGISTRY_REGISTER( senf::EtherOUIExtTypes,
    senf::EtherOUIExtTypes::type(senf::EthOUIExtensionPacketType::OUI_Fraunhofer_FIT, 0x43), senf::emu::EthernetFragmentPacket);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::EthernetFragmentPacket );

prefix_ void senf::emu::EthernetFragmentPacketType::dump(packet p, std::ostream & os)
{
    os << "SENF/NetEMU EthernetFragment Packet:" << std::endl
       << senf::fieldName("fragmentNr")   << p->fragmentNr() << std::endl
       << senf::fieldName("moreFragment") << unsigned(p->moreFragment()) << std::endl
       << senf::fieldName("size")         << p->size() << std::endl
       << senf::fieldName("type/length")  << senf::format::dumpint(p->type_length().value()) << std::endl;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
