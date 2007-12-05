// $id: EthernetPacket.hh 299 2007-07-10 21:23:49Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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

#ifndef HH_LlcSnapPacket_
#define HH_LlcSnapPacket_ 1

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
    struct Parse_LlcSnapPacket : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( dsap, Parse_UInt8 );
        SENF_PARSER_FIELD( ssap, Parse_UInt8 );
        SENF_PARSER_FIELD( ctrl, Parse_UInt8 );

        SENF_PARSER_FIELD( protocolId, Parse_UInt24 );
        SENF_PARSER_FIELD( type_length, Parse_UInt16 );

        SENF_PARSER_FINALIZE(Parse_LlcSnapPacket);
        
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
            \ref Parse_LlcSnapPacket

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            XXXX

        \ingroup protocolbundle_default
     */
    struct LlcSnapPacketType
        : public PacketTypeBase, 
          public PacketTypeMixin<LlcSnapPacketType, EtherTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<LlcSnapPacketType, EtherTypes> mixin;
        typedef ConcretePacket<LlcSnapPacketType> packet;
        typedef Parse_LlcSnapPacket parser;
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
