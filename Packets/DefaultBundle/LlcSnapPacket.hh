// $Id$
//
// Copyright (C) 2007
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
    \brief LLC/SNAP public header */

#ifndef HH_SENF_Packets_DefaultBundle_LlcSnapPacket_
#define HH_SENF_Packets_DefaultBundle_LlcSnapPacket_ 1

// Custom includes
#include <algorithm>
#include "EthernetPacket.hh"
#include "../../Packets/Packets.hh"

//#include "LlcSnapPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a LLC/SNAP header
        
        \todo document me
     */
    struct LlcSnapPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( dsap, UInt8Parser );
        SENF_PARSER_FIELD( ssap, UInt8Parser );
        SENF_PARSER_FIELD( ctrl, UInt8Parser );

        SENF_PARSER_FIELD( protocolId, UInt24Parser );
        SENF_PARSER_FIELD( type_length, UInt16Parser );

        SENF_PARSER_FINALIZE(LlcSnapPacketParser);
        
        SENF_PARSER_INIT() {
            dsap() = 0xaa;
            ssap() = 0xaa;
            ctrl() = 0x03;
            protocolId() = 0x000000;
        }
    };

    /** \brief LLC/SNAP header

        \todo document me

        \par Packet type (typedef):
            \ref LlcSnapPacketType

        \par Fields:
            \ref LlcSnapPacketParser

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            XXXX

        \image html LlcSnapPacket.png

        \ingroup protocolbundle_default
     */
    struct LlcSnapPacketType
        : public PacketTypeBase, 
          public PacketTypeMixin<LlcSnapPacketType, EtherTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<LlcSnapPacketType, EtherTypes> mixin;
        typedef ConcretePacket<LlcSnapPacketType> packet;
        typedef LlcSnapPacketParser parser;
#endif
        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;
        
        static factory_t nextPacketType(packet p);
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    typedef ConcretePacket<LlcSnapPacketType> LlcSnapPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "LlcSnapPacket.cci"
//#include "LlcSnapPacket.ct"
//#include "LlcSnapPacket.cti"
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
