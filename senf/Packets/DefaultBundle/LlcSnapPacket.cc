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
    \brief LlcSnapPacket non-inline non-template implementation */

#include "LlcSnapPacket.hh"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "EthernetPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::LlcSnapPacket );

prefix_ void senf::LlcSnapPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "LLC/SNAP:\n"
       << std::hex << std::setfill('0') << std::right
       << "  LLC\n"
       << senf::fieldName("  dsap")        << "0x" << unsigned(p->dsap()) << "\n"
       << senf::fieldName("  ssap")        << "0x" << unsigned(p->ssap()) << "\n"
       << senf::fieldName("  control id")  << "0x" << unsigned(p->ctrl()) << "\n"
       << "  SNAP\n"
       << senf::fieldName("  protocol id") << "0x" << std::setw(6) << unsigned(p->protocolId()) << "\n"
       << senf::fieldName("  type/length") << "0x" << std::setw(4) << unsigned(p->type_length()) << "\n";
}

prefix_ senf::PacketInterpreterBase::factory_t senf::LlcSnapPacketType::nextPacketType(packet p)
{
    if (p->type_length() >= 1536) {
        PacketRegistry<senf::EtherTypes>::Entry const * e (
            PacketRegistry<senf::EtherTypes>::lookup( p->type_length(), nothrow ));
        return e ? e->factory() : no_factory();
    }
    if (p->type_length() <= 1500)
        return EthernetPacket::factory();
    return no_factory();
}

prefix_ void senf::LlcSnapPacketType::finalize(packet p)
{
    p->type_length() << key(p.next(nothrow));
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
