// $Id:DatagramSection.hh 327 2007-07-20 10:03:44Z tho $
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
    \brief TransportPacket public header */

#ifndef HH_TransportPacket_
#define HH_TransportPacket_ 1

// Custom includes
#include <algorithm>
#include "Packets/PacketType.hh"
#include "Packets/ParseInt.hh"
#include "Packets/PacketRegistry.hh"
#include "Packets/PacketParser.hh"

//#include "TransportPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a Transport Stream packet

        Parser implementing the header of a MPEG Transport Stream Packet.
        
        \see TransportPacketType
     */
    struct Parse_TransportPacket : public PacketParserBase
    {
        typedef Parse_Flag      <     0 > Parse_tei; // transport_error_indicator
        typedef Parse_Flag      <     1 > Parse_pusi; // payload_unit_start_indicator
        typedef Parse_Flag      <     2 > Parse_transportPrio;  // transport_priority
        typedef Parse_UIntField < 2, 16 > Parse_pid;
        typedef Parse_UIntField < 0,  2 > Parse_tsc; // transport_scrambling_control
        typedef Parse_UIntField < 2,  4 > Parse_adaptCtrl; // adaptation_field_control
        typedef Parse_UIntField < 4,  8 > Parse_contCounter; // continuity_counter
    
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_INIT(Parse_TransportPacket);
        
        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field       ) ( sync_byte,                 Parse_UInt8         )) 
            ((OverlayField) ( transport_error_indicator, Parse_tei           ))
            ((OverlayField) ( pusi,                      Parse_pusi          ))
            ((OverlayField) ( transport_priority,        Parse_transportPrio ))
            ((Field       ) ( pid,                       Parse_pid           ))
            ((OverlayField) ( transport_scrmbl_ctrl,     Parse_tsc           ))
            ((OverlayField) ( adaptation_field_ctrl,     Parse_adaptCtrl     ))
            ((Field       ) ( continuity_counter,        Parse_contCounter   ))      
        );
        
#       else
        
        Parse_UInt8         sync_byte() const;
        Parse_tei           transport_error_indicator() const;
        Parse_pusi          pusi() const;
        Parse_transportPrio transport_priority() const;
        Parse_pid           pid() const;
        Parse_tsc           transport_scrmbl_ctrl() const;
        Parse_adaptCtrl     adaptation_field_ctrl() const;
        Parse_contCounter   continuity_counter() const;

#       endif
        
//        Parse_UInt8 payload_pointer() const {
//            return parse<Parse_UInt8>( Parse_TransportPacket::fixed_bytes ); 
//        }
    };
    
    /** \brief Transport Stream packet
        
        \par Packet type (typedef):
            \ref TransportPacket

        \par Fields:
            \ref Parse_TransportPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct TransportPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TransportPacketType>
    {
        typedef PacketTypeMixin<TransportPacketType> mixin;
        typedef ConcretePacket<TransportPacketType> packet;
        typedef Parse_TransportPacket parser;
    
        using mixin::nextPacketRange;
    //          using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
    };
    
    /** \brief Transport packet typedef */
    typedef TransportPacketType::packet TransportPacket;
    
    #define TRANSPORT_PACKET_SYNC_BYTE 0x47
  
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TransportPacket.cci"
//#include "TransportPacket.ct"
//#include "TransportPacket.cti"
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
