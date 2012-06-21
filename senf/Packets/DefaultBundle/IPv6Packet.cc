// $Id$
//
// Copyright (C) 2007
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
    \brief IPv6Packet non-inline non-template implementation */

#include "IPv6Packet.hh"
//#include "IPv6Packet.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "EthernetPacket.hh"

//#include "IPv6Packet.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::IPv6PacketType::etherType, senf::IPv6Packet);
    SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes,    41,                              senf::IPv6Packet); // IP6-in-IP(6) encapsulation
}

prefix_ void senf::IPv6PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Internet protocol Version 6:\n"
       << senf::fieldName("version")                   << unsigned(p->version()) << "\n"
       << senf::fieldName("traffic class")
       << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::right << unsigned(p->trafficClass()) << "\n"
       << senf::fieldName("flow label")
       << "0x" << std::hex << std::setw(5) << std::setfill('0') << std::right << unsigned(p->flowLabel()) << "\n"
       << senf::fieldName("payload length")            << std::dec << unsigned(p->length()) << "\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("hop limit")                 << unsigned(p->hopLimit()) << "\n"
       << senf::fieldName("source")                    << p->source() << "\n"
       << senf::fieldName("destination")               << p->destination() << "\n";
}

prefix_ void senf::IPv6PacketType::finalize(packet p)
{
    p->length() << (p.size() - IPv6PacketParser::fixed_bytes);
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "IPv6Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
