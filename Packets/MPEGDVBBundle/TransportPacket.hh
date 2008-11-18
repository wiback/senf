// $Id:TransportPacket.hh 560 2007-12-13 14:39:37Z tho $
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
    \brief TransportPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_TransportPacket_
#define HH_SENF_Packets_MPEGDVBBundle_TransportPacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"

//#include "TransportPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

//    struct PSIPayloadPacketParser : public PacketParserBase
//    {
//        static const size_type fixed_bytes = 184;
//
//    };


    /** \brief Parse a Transport Stream packet

        Parser implementing the header of a MPEG Transport Stream packet.
        
        \see TransportPacketType
     */
    struct TransportPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD       ( sync_byte,                  UInt8Parser );

        SENF_PARSER_BITFIELD    ( transport_error_indicator,  1, bool     );
        SENF_PARSER_BITFIELD_RO ( pusi,                       1, bool     );
        SENF_PARSER_BITFIELD    ( transport_priority,         1, bool     );
        SENF_PARSER_BITFIELD    ( pid,                       13, unsigned );
        SENF_PARSER_BITFIELD    ( transport_scrmbl_ctrl,      2, unsigned );
        SENF_PARSER_BITFIELD    ( adaptation_field_ctrl,      2, unsigned );
        SENF_PARSER_BITFIELD    ( continuity_counter,         4, unsigned );

        SENF_PARSER_PRIVATE_VARIANT ( pointer_field_, pusi,
                (senf::VoidPacketParser) (UInt8Parser) );
        
        SENF_PARSER_FINALIZE( TransportPacketParser );
        
        UInt8Parser pointer_field() const;
        void init_fields() const;
        void setPUSI(bool pusi) const;

        SENF_PARSER_INIT() {
            defaultInit();
            init_fields();
        }
    };
    
    /** \brief Transport Stream packet
        
        <table class="senf">
          <tr style="text-align:center">
            <th>Syntax</th><th>No. of bits</th></tr>
          <tr>
            <td>transport_packet() {</td> <td></td>
          </tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::sync_byte() "sync_byte"</td>
            <td>8</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::transport_error_indicator() "transport_error_indicator"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::pusi() "payload_uni_start_indicator"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::transport_priority() "transport_priority"</td>
            <td>1</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::pid() "PID"</td>
            <td>13</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::transport_scrmbl_ctrl() "transport_scrambling_control"</td>
            <td>2</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::adaptation_field_ctrl() "adaptation_field_control"</td>
            <td>2</td></tr>
          <tr>
            <td style="padding-left:2em">\ref TransportPacketParser::continuity_counter() "continuity_counter"</td>
            <td>4</td></tr>
          <tr>
            <td>}</td> <td></td></tr>
        </table>
        
        \par Packet type (typedef):
            \ref TransportPacket

        \par Fields:
            \ref TransportPacketParser

        \ingroup protocolbundle_mpegdvb
     */
    struct TransportPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TransportPacketType>
    {
        typedef PacketTypeMixin<TransportPacketType> mixin;
        typedef ConcretePacket<TransportPacketType> packet;
        typedef TransportPacketParser parser;
    
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
        static const byte SYNC_BYTE = 0x47;
    };
    
    /** \brief Transport packet typedef */
    typedef ConcretePacket<TransportPacketType> TransportPacket;
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
