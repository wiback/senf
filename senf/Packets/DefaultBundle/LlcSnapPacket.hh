// $Id$
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
    \brief LLC/SNAP public header */

#ifndef HH_SENF_Packets_DefaultBundle_LlcSnapPacket_
#define HH_SENF_Packets_DefaultBundle_LlcSnapPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse a LLC/SNAP header

        \image html LlcSnapPacket.png
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

        \ingroup protocolbundle_default
     */
    struct LlcSnapPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<LlcSnapPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<LlcSnapPacketType, EtherTypes> mixin;
        typedef ConcretePacket<LlcSnapPacketType> packet; ///< LLC/SNAP packet typedef
        typedef LlcSnapPacketParser parser;               ///< typedef to the parser of LLC/SNAP packet

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /** \brief Dump given LlcSnapPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief LLC/SNAP packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<LlcSnapPacketType> LlcSnapPacket;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
