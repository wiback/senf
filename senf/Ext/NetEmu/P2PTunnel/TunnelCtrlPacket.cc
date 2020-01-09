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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

/** \file
    \brief TunnelControlPacket non-inline non-template implementation */

#include "TunnelCtrlPacket.hh"

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherOUIExtTypes,
        senf::EtherOUIExtTypes::type(senf::EthOUIExtensionPacketType::OUI_Fraunhofer_FIT, 0x42), senf::emu::TunnelCtrlPacket);

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::TunnelCtrlPacket);


prefix_ void senf::emu::TunnelCtrlPacketType::dump(packet p, std::ostream & os)
{
    static const char * cmdCodes[] = {
            "invalid", "SetupRequest", "SetupAck", "SetupReject", "TerminateRequest",
            "TerminateAck", "EchoRequest", "EchoReply" };
    os << "SENF NetEmu Tunnel Control Packet:\n"
       << senf::fieldName("Code") << unsigned(p->code()) << " ("
       << (p->code() <= TunnelCtrlPacketParser::EchoReply ? cmdCodes[p->code()] : "unknown")
       << ")" << std::endl;
    if (p->has_capacity()) {
        os << "Capacity:" << std::endl
           << "  from client to server: " << p->capacity().fromClientToServer() << std::endl
           << "  from server to client: " << p->capacity().fromServerToClient() << std::endl;
    }

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
