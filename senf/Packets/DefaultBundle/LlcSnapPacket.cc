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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief LlcSnapPacket non-inline non-template implementation */

#include "LlcSnapPacket.hh"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "EthernetPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
