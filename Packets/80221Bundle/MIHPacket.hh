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
    \brief MIH protocol public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHPacket_
#define HH_SENF_Packets_80221Bundle_MIHPacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"

//#include "MIHPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct MIHPacketParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        
        SENF_PARSER_BITFIELD  ( version,       4,  unsigned );
        SENF_PARSER_BITFIELD  ( ackRequest,    1,  bool     );
        SENF_PARSER_BITFIELD  ( ackResponse,   1,  bool     );
        SENF_PARSER_BITFIELD  ( uir,           1,  bool     );
        SENF_PARSER_BITFIELD  ( moreFragment,  1,  bool     );
        SENF_PARSER_BITFIELD  ( fragmentNr,    7,  unsigned );
        SENF_PARSER_SKIP_BITS ( 1                           );
        
        // MIH message ID (MID)
        SENF_PARSER_BITFIELD  ( sid,           4,  unsigned );
        SENF_PARSER_BITFIELD  ( opcode,        2,  unsigned );
        SENF_PARSER_BITFIELD  ( aid,           10, unsigned );
        
        SENF_PARSER_SKIP_BITS ( 4                           );
        SENF_PARSER_BITFIELD  ( transactionId, 12, unsigned );
        SENF_PARSER_FIELD_RO  ( payloadLength, UInt16Parser );
        
        SENF_PARSER_FINALIZE ( MIHPacketParser );
    };
    
    
    struct MIHPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHPacketType>
    {
        typedef PacketTypeMixin<MIHPacketType> mixin;
        typedef ConcretePacket<MIHPacketType> packet;
        typedef MIHPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream &os);
//        static void finalize(packet p);
    };

    typedef ConcretePacket<MIHPacketType> MIHPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
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
