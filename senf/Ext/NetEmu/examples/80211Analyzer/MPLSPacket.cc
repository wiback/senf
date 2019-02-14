//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//

#include "MPLSPacket.hh"

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include "TIMPacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

// downstream assigned label - RFC 5331 (default)
SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::MPLSPacketType::etherType, senf::MPLSPacket);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MPLSPacket );

prefix_ void senf::MPLSPacketType::dump(packet p, std::ostream & os)
{
    os << "MPLS Header: \n"
       << "  label : " << p->label() << "\n"
       << "  tc : " << p->tc() << "\n"
       << "  bottom of stack : " << p->s() << "\n"
       << "  Time-to-live : " << unsigned(p->ttl()) << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
