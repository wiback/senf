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
#include "../../Packets/Packets.hh"

//#include "TransportPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a Transport Stream packet

        Parser implementing the header of a MPEG Transport Stream packet.
        
        \see TransportPacketType
     */
    struct Parse_TransportPacket : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD    ( sync_byte                 , Parse_UInt8   );
        
        SENF_PARSER_BITFIELD ( transport_error_indicator ,  1 , bool     );
        SENF_PARSER_BITFIELD ( pusi                      ,  1 , bool     );
        SENF_PARSER_BITFIELD ( transport_priority        ,  1 , bool     );
        SENF_PARSER_BITFIELD ( pid                       , 13 , unsigned );
        SENF_PARSER_BITFIELD ( transport_scrmbl_ctrl     ,  2 , unsigned );
        SENF_PARSER_BITFIELD ( adaptation_field_ctrl     ,  2 , unsigned );
        SENF_PARSER_BITFIELD ( continuity_counter        ,  4 , unsigned );

        SENF_PARSER_FINALIZE( Parse_TransportPacket );
    
//        Parse_UInt8 payload_pointer() const {
//            return parse<Parse_UInt8>( Parse_TransportPacket::fixed_bytes ); 
//        }
    };
    
    /** \brief Transport Stream packet
        
        <table class="senf">
          <tr style="text-align:center">
            <th>Syntax</th><th>No. of bits</th></tr>
          <tr>
            <td>transport_packet() {</td> <td></td>
          </tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::sync_byte() "sync_byte"</td>
            <td>8</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::transport_error_indicator() "transport_error_indicator"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::pusi() "payload_uni_start_indicator"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::transport_priority() "transport_priority"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::pid() "PID"</td>
            <td>13</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::transport_scrmbl_ctrl() "transport_scrambling_control"</td>
            <td>2</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::adaptation_field_ctrl() "adaptation_field_control"</td>
            <td>2</td></tr>
          <tr>
            <td style="padding-left:2em">\ref Parse_TransportPacket::continuity_counter() "continuity_counter"</td>
            <td>4</td></tr>
          <tr>
            <td>}</td> <td></td></tr>
        </table>
        
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
