//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>
//

#include "InternalThroughputTestPacket.hh"

// Custom includes
#include <senf/Packets/DefaultBundle/Registries.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherOUIExtTypes,
    // Fraunhofer FIT OUI
    senf::EtherOUIExtTypes::type(senf::EthOUIExtensionPacketType::OUI_Fraunhofer_FIT, emu::InternalThroughputTestPacketType::extType), emu::InternalThroughputTestPacket);

prefix_ void emu::InternalThroughputTestPacketType::dump(packet p, std::ostream & os)
{
    os << "EMU Internal Throughput Test Packet:\n"
       << "sessionId "   << p->sessionId()
       << ", seqNo "     << p->seqNo()
       << ", numPacket " << p->numPkts()
       << ", magic "     << p->magic()
       << ", timestamp " << p->timestamp();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
