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
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Statistics.hh>
#include <senf/Ext/NetEmu/Annotations.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    struct TIMPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( syn,             1, bool     );
        SENF_PARSER_BITFIELD( oddSize,         1, bool     );
        SENF_PARSER_BITFIELD( padding,         4, unsigned );
        SENF_PARSER_BITFIELD( timestamp,      14, unsigned );
        SENF_PARSER_BITFIELD( sequenceNumber, 22, unsigned );
        SENF_PARSER_BITFIELD( linkLocalSeqNo, 22, unsigned );

        SENF_PARSER_FINALIZE(TIMPacketParser);
    };

    struct TIMPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TIMPacketType>
    {
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

    //
    // TIM SeqNo Stats Analyzer
    //
    struct TIMSeqNoStats {
        std::uint32_t last_;
        std::uint32_t good;
        std::uint32_t goodBytes;
        std::uint32_t duplicate;
        std::uint32_t late;
        std::uint32_t lost;
        std::uint32_t resyncs;
        std::uint64_t goodTotal;
        std::uint64_t goodBytesTotal;

        TIMSeqNoStats();
        TIMSeqNoStats(TIMPacket const & tim, bool llSeq = false);
        virtual ~TIMSeqNoStats() {};

        virtual void clear();
        void reset();

        void countGood(std::uint32_t payloadSize);
        bool process(std::uint32_t seqNo, std::uint32_t payloadSize);
        bool processSeqNo(TIMPacket const & tim);
        bool processLLSeqNo(TIMPacket const & tim);

        void dump(std::ostream & os, senf::ClockService::clock_type const & period = senf::ClockService::clock_type(0));
    };

    struct TIMTimestampStats {
        senf::StatisticAccumulator<std::uint32_t> delay;
        senf::StatisticAccumulator<std::uint32_t> pdv;
        // no unsigned here!
        std::int32_t lastPD_;

        TIMTimestampStats();

        void clear();
        void reset();
        void process(TIMPacket const & tim);
    };
}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::TIMPacket );

///////////////////////////////hh.e////////////////////////////////////////
#include "TIMPacket.cci"
//#include "TIMPacket.ct"
//#include "TIMPacket.cti"
#endif
