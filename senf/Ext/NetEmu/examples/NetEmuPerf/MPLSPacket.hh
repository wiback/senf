//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//

#ifndef HH_WiBACK_DataPlane_Packets_MPLSPacket_
#define HH_WiBACK_DataPlane_Packets_MPLSPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    struct MPLSPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( label, 20, unsigned);
        SENF_PARSER_BITFIELD( tc, 3, unsigned);
        SENF_PARSER_BITFIELD_RO( s, 1, bool);
        SENF_PARSER_FIELD( ttl, UInt8Parser);

        SENF_PARSER_FINALIZE(MPLSPacketParser);

        friend struct MPLSPacketType;
    };

    struct MPLSPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MPLSPacketType>
    {
        typedef PacketTypeMixin<MPLSPacketType> mixin;
        typedef ConcretePacket<MPLSPacketType> packet;
        typedef MPLSPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given MPLSPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        static constexpr std::uint16_t etherType = 0x8847;
    };

    typedef ConcretePacket<MPLSPacketType> MPLSPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::MPLSPacket );
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
