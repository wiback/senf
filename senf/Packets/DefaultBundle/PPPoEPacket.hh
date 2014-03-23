// $Id: PPPoEPacket.hh 2056 2014-03-14 13:59:24Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief PPPOE public header */

#ifndef HH_SENF_Packets_DefaultBundle_PPPoEPacket_
#define HH_SENF_Packets_DefaultBundle_PPPoEPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse a PPPoE header

        \image html PPPoEPacket.png
        \todo document me
     */
    struct PPPoEPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( version, 4, unsigned );
        SENF_PARSER_BITFIELD( type, 4,    unsigned );
        SENF_PARSER_FIELD( code,          UInt8Parser );
        SENF_PARSER_FIELD( sessionId,     UInt16Parser );

        SENF_PARSER_FIELD( length,        UInt16Parser );
        SENF_PARSER_FIELD( pppProtocol,   UInt16Parser );

        SENF_PARSER_FINALIZE( PPPoEPacketParser );

        SENF_PARSER_INIT() {
            version() = 1;
            type() = 1;
            code() = 0;
            sessionId() = 0x0000;
        }
    };

    /** \brief PPPoE header

        \todo document me

        \par Packet type (typedef):
            \ref PPPoEPacketType

        \par Fields:
            \ref PPPoEPacketParser

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            XXXX

        \ingroup protocolbundle_default
     */
    struct PPPoEPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<PPPoEPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<PPPoEPacketType, EtherTypes> mixin;
        typedef ConcretePacket<PPPoEPacketType> packet; ///< PPPoE packet typedef
        typedef PPPoEPacketParser parser;               ///< typedef to the parser of PPPoE packet

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /** \brief Dump given PPPoEPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
    };

    /** \brief PPPoE packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<PPPoEPacketType> PPPoEPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( PPPoEPacket );
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "PPPoEPacket.cci"
//#include "PPPoEPacket.ct"
//#include "PPPoEPacket.cti"
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
