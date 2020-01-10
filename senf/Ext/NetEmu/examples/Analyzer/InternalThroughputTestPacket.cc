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
