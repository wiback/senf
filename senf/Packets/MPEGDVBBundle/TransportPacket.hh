// $Id:TransportPacket.hh 560 2007-12-13 14:39:37Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief TransportPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_TransportPacket_
#define HH_SENF_Packets_MPEGDVBBundle_TransportPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "TransportPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse a Transport Stream packet

        Parser implementing the header of a MPEG Transport Stream packet.
        \image html TransportPacket.png

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
        typedef ConcretePacket<TransportPacketType> packet; ///< Transport packet typedef
        typedef TransportPacketParser parser; ///< typedef to the parser of Transport packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        /** \brief Dump given Transport packet in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
        static const byte SYNC_BYTE = 0x47;
    };

    /** \brief Transport packet typedef */
    typedef ConcretePacket<TransportPacketType> TransportPacket;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
