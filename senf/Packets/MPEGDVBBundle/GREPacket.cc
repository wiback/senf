// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Joachim Kaeber <jkaeber@berlios.de>
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
    \brief GREPacket non-inline non-template implementation */

#include "GREPacket.hh"
//#include "GREPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <iomanip>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, 0x6558, senf::EthernetPacket );
SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 47, senf::GREPacket );

prefix_ void senf::GREPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "GRE Encapsulation:\n"
       <<     "  checksum_present        : " << p->checksum_present() << "\n"
       <<     "  protocol_type           : 0x" << std::hex << p->protocol_type() << "\n"
       ;
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
