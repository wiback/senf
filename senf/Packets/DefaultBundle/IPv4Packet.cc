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
    \brief IPv4Packet non-inline non-template implementation */

#include "IPv4Packet.hh"
//#include "IPv4Packet.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/IpChecksum.hh>
#include "Registries.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::IPv4PacketType::etherType, senf::IPv4Packet);
    SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes,    4,                               senf::IPv4Packet); // IP-in-IP encapsulation
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv4PacketParser

prefix_ boost::uint16_t senf::IPv4PacketParser::calcChecksum()
    const
{
    validate(bytes(*this));
    IpChecksum summer;
    summer.feed( i(),                   i()+checksum_offset );
    summer.feed( i()+checksum_offset+2, i()+bytes(*this)    );
    return summer.sum();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv4PacketType

prefix_ void senf::IPv4PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Internet protocol Version 4:\n"
       << senf::fieldName("version")                   << p->version() << "\n"
       << senf::fieldName("ip header length")          << p->ihl() << "\n"
       << senf::fieldName("tos")                       << unsigned(p->tos()) << "\n"
       << senf::fieldName("length")                    << p->length() << "\n"
       << senf::fieldName("identifier")                << p->identifier() << "\n"
       << senf::fieldName("dont fragment")             << p->df() << "\n"
       << senf::fieldName("more fragments")            << p->mf() << "\n"
       << senf::fieldName("fragment")                  << p->frag() << "\n"
       << senf::fieldName("ttl")                       << unsigned(p->ttl()) << "\n"
       << senf::fieldName("protocol")                  << unsigned(p->protocol()) << "\n"
       << senf::fieldName("checksum")
       << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::right << p->checksum() << std::dec << "\n"
       << senf::fieldName("source")                    << p->source() << "\n"
       << senf::fieldName("destination")               << p->destination() << "\n";
}

prefix_ void senf::IPv4PacketType::finalize(packet p)
{
    p->length()   << p.size();
    p->protocol() << key(p.next(nothrow));
    p->checksum() << p->calcChecksum();
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
