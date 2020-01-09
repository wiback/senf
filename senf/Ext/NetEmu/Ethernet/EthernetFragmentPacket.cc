// $Id:$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

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
