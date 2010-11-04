// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIH Message-Registry public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHMessageRegistry_
#define HH_SENF_Packets_80221Bundle_MIHMessageRegistry_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "MIHPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct MIHMessageRegistry {
        // MIH messages registry
        typedef boost::uint16_t key_t;
    };

#   define SENF_MIH_PACKET_REGISTRY_REGISTER( packet )                    \
        SENF_PACKET_REGISTRY_REGISTER(                                    \
            senf::MIHMessageRegistry, packet::type::MESSAGE_ID, packet )

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MIHPacket.cci"
//#include "MIHPacket.ct"
//#include "MIHPacket.cti"
#endif



// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
