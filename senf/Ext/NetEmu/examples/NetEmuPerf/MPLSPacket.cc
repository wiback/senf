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

prefix_ senf::PacketInterpreterBase::factory_t senf::MPLSPacketType::nextPacketType(packet p)
{
    if (p->s())  // last MPLS header
        return wiback::TIMPacket::factory();
    else
        return MPLSPacket::factory();
}

prefix_ void senf::MPLSPacketType::finalize(packet p)
{
    if (p.next(senf::nothrow).is<MPLSPacket>())  // not the last label on the MPLS stack
        p->s_() << false;
    else
        p->s_() << true;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
