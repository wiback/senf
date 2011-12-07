// $Id$
//
// Copyright (C) 2009
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
    \brief MIH protocol public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHPacket_
#define HH_SENF_Packets_80221Bundle_MIHPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "TLVParser.hh"
#include "MIHMessageRegistry.hh"

//#include "MIHPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse a MIH packet

        Parser implementing the MIH header. The fields implemented are:
        \image html MIHPacket.png

        \see MIHPacketType
     */
    struct MIHPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()

        SENF_PARSER_BITFIELD_RO ( version,       4,  unsigned );
        SENF_PARSER_BITFIELD    ( ackRequest,    1,  bool     );
        SENF_PARSER_BITFIELD    ( ackResponse,   1,  bool     );
        SENF_PARSER_BITFIELD    ( uir,           1,  bool     );
        SENF_PARSER_BITFIELD    ( moreFragment,  1,  bool     );
        SENF_PARSER_BITFIELD    ( fragmentNr,    7,  unsigned );
        SENF_PARSER_SKIP_BITS   ( 1                           );

        // MIH message ID (MID)
        SENF_PARSER_FIELD    ( messageId, UInt16Parser ); //<pkgdraw:hide
        SENF_PARSER_GOTO     ( messageId               );
        SENF_PARSER_BITFIELD ( sid,     4,  unsigned   );
        SENF_PARSER_BITFIELD ( opcode,  2,  unsigned   );
        SENF_PARSER_BITFIELD ( aid,    10,  unsigned   );

        SENF_PARSER_SKIP_BITS ( 4                           );
        SENF_PARSER_BITFIELD  ( transactionId, 12, unsigned );
        SENF_PARSER_FIELD_RO  ( payloadLength, UInt16Parser );

        SENF_PARSER_GOTO_OFFSET( 8, 8); // just to limit the offset calculation

        // Source MIHF Id
        SENF_PARSER_FIELD ( src_mihfId, MIHFSrcIdTLVParser );
        // Destination MIHF Id
        SENF_PARSER_FIELD ( dst_mihfId, MIHFDstIdTLVParser );

        SENF_PARSER_FINALIZE ( MIHPacketParser );

        SENF_PARSER_INIT() {
            defaultInit();
            version_() = 1;
        }

        friend struct MIHPacketType;
    };

    /** \brief MIH packet

        \par Packet type (typedef):
            \ref MIHPacket

        \par Fields:
            \ref MIHPacketParser

        \ingroup protocolbundle_80221
     */
    struct MIHPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHPacketType, MIHMessageRegistry>
    {
        typedef PacketTypeMixin<MIHPacketType, MIHMessageRegistry> mixin;
        typedef ConcretePacket<MIHPacketType> packet; ///< MIH packet typedef
        typedef MIHPacketParser parser;               ///< typedef to the parser of MIH packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        /** \brief Dump given MIH packet in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
        static factory_t nextPacketType(packet p);
        static void validate(packet p);

        static const boost::uint16_t etherType = 0x8917;
    };

    /** \brief MIH packet typedef
        \ingroup protocolbundle_80221
     */
    typedef ConcretePacket<MIHPacketType> MIHPacket;


    struct MIHGenericPayloadPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_LIST ( tlvList, packetSize(), MIHGenericTLVParser );
        SENF_PARSER_FINALIZE ( MIHGenericPayloadPacketParser );
    };

    struct MIHGenericPayloadPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHGenericPayloadPacketType>
    {
        typedef PacketTypeMixin<MIHGenericPayloadPacketType> mixin;
        typedef ConcretePacket<MIHGenericPayloadPacketType> packet; ///< MIH Payload packet typedef
        typedef MIHGenericPayloadPacketParser parser; ///< typedef to the parser of MIH Payload packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        /** \brief Dump given MIHGenericPayload in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

     /** \brief MIH Payload packet typedef
         \ingroup protocolbundle_80221
      */
    typedef ConcretePacket<MIHGenericPayloadPacketType> MIHGenericPayloadPacket;
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
