// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief EthernetPacket non-inline non-template implementation */

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include "LlcSnapPacket.hh"
#include <iomanip>
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacket>
        registerEthVLanPacket(0x8100);
}

///////////////////////////////////////////////////////////////////////////
// senf::EthernetPacketType

prefix_ void senf::EthernetPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    if (p->type_length() <= 1500)
        os << "Ethernet 802.3";
    else if (p->type_length() >= 0x600)
        os << "Ethernet II (DIX)";
    else
        os << "Ethernet 802.3 (bad ethertype >1500 and <1536)";
    os << ": \n"
       << "  destination : " << p->destination() << "\n"
       << "  source      : " << p->source() << "\n"
       << "  type/length : 0x" 
       << std::hex << std::setw(4) << std::setfill('0') << p->type_length() << "\n";
}

prefix_ senf::PacketInterpreterBase::factory_t senf::EthernetPacketType::nextPacketType(packet p)
{
    if      (p->type_length() >= 1536) return lookup(p->type_length());
    else if (p->type_length() <= 1500) return LlcSnapPacket::factory();
    else                               return no_factory();
}

prefix_ void senf::EthernetPacketType::finalize(packet p)
{
    Packet n (p.next(nothrow));
    if (n) {
        optional_key_t k (key(n));
        if (k)
            p->type_length() << k;
        else if (n.is<LlcSnapPacket>())
            p->type_length() << n.data().size();
    }
    // Do NOT reset type_length if the type is not known ... doing this will destroy read packets
}

prefix_ void senf::EthVLanPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Ethernet 802.1q (VLAN):\n"
       << "  priority      : " << p->priority() << "\n"
       << "  cfi           : " << p->cfi() << "\n"
       << "  vlan-ID       : " << p->vlanId() << "\n"
       << "  ethertype     : 0x" 
       << std::hex << std::setw(4) << std::setfill('0') << p->type() << "\n";
}

prefix_ void senf::EthVLanPacketType::finalize(packet p)
{
    p->type() << key(p.next(nothrow));
}


///////////////////////////////cc.e////////////////////////////////////////
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
