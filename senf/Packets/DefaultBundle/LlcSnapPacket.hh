//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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

        SENF_PARSER_FINALIZE( LlcSnapPacketParser );

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
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( LlcSnapPacket );
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
