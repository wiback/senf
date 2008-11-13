// $Id$
//
// Copyright (C) 2007
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
    \brief DataPacket public header */

#ifndef HH_DataPacket_
#define HH_DataPacket_ 1

// Custom includes
#include "PacketType.hh"
#include "Packet.hh"

//#include "DataPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Generic payload-only packet

        \par Packet type (typedef):
            \ref DataPacket

        DataPacket is a simple generic packet with just a payload of uninterpreted data. This is the
        packet used whenever no more specific packet type can be found for a sub-packet (e.g. as the
        payload data of a UDP packet)

        \ingroup packet_module
     */
    struct DataPacketType : public PacketTypeBase
    {
        typedef ConcretePacket<DataPacketType> packet;
        static void dump(packet p, std::ostream & os);
    };

    /** \brief Generic payload-only packet typedef */
    typedef ConcretePacket<DataPacketType> DataPacket;
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_DataPacket_impl_)
#define HH_DataPacket_impl_
//#include "DataPacket.cci"
//#include "DataPacket.ct"
//#include "DataPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:

