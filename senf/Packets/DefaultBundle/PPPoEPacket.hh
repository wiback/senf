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

    template <typename PacketType>
    struct PPPoEPacketTypeBase
        : public PacketTypeBase,
          public PacketTypeMixin<PacketType, EtherTypes>
    {
        typedef PacketTypeMixin<PacketType, EtherTypes> mixin;
        typedef ConcretePacket<PacketType> packet;
        typedef PPPoEPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        static void dump(packet p, std::ostream & os);
    };

    struct PPPoEDPacketType : public PPPoEPacketTypeBase<PPPoEDPacketType> {
        static const EtherTypes::key_t etherType = 0x8863;

    };
    struct PPPoESPacketType : public PPPoEPacketTypeBase<PPPoESPacketType> {
        static const EtherTypes::key_t etherType = 0x8864;
    };

    typedef ConcretePacket<PPPoEDPacketType> PPPoEDPacket;
    typedef ConcretePacket<PPPoESPacketType> PPPoESPacket;

    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( PPPoEDPacket );
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( PPPoESPacket );
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
