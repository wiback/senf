//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//

#ifndef HH_WiBACK_DataPlane_Packets_TIMPacket_
#define HH_WiBACK_DataPlane_Packets_TIMPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace wiback {

    struct TIMPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( syn,             1, bool     );
        SENF_PARSER_BITFIELD( oddSize,         1, bool     );
        SENF_PARSER_BITFIELD( timestamp,      14, unsigned );
        SENF_PARSER_BITFIELD( sequenceNumber, 24, unsigned );
        SENF_PARSER_BITFIELD( linkLocalSeqNo, 24, unsigned );

        SENF_PARSER_FINALIZE(TIMPacketParser);
    };

    struct TIMPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TIMPacketType>
    {
        static constexpr std::uint32_t TSBits    = 14;
        static constexpr std::uint32_t SeqNoBits = 24;

        typedef senf::PacketTypeMixin<TIMPacketType> mixin;
        typedef senf::ConcretePacket<TIMPacketType> packet;
        typedef TIMPacketParser parser;

        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        static optional_range nextPacketRange(packet const & p);
        /// Dump given TIMPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
        // computes the difference between our timestamps and seqNos
        static std::int32_t timeStampDiff(std::uint32_t tstamp2, std::uint32_t tstamp1);
        static std::int32_t seqNoDiff(std::uint32_t current, std::uint32_t last);
    };

    typedef senf::ConcretePacket<TIMPacketType> TIMPacket;
}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( wiback::TIMPacket );

///////////////////////////////hh.e////////////////////////////////////////
#include "TIMPacket.cci"
//#include "TIMPacket.ct"
//#include "TIMPacket.cti"
#endif
