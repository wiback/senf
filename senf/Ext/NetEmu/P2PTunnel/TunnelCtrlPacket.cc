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
