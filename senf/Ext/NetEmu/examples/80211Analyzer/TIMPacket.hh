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


#ifndef TIMPacket_
#define TIMPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Utils/FlowStatistics.hh>
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
    class TIMSeqNoStats
        : public senf::SequenceNumberStatistics
    {
    public:
        TIMSeqNoStats();
        TIMSeqNoStats(TIMPacket const & tim, bool llSeq = false);

        bool processSeqNo(TIMPacket const & tim);
        bool processLLSeqNo(TIMPacket const & tim);
    };

    class TIMTimestampStats
        : public senf::TimestampStatistics
    {
    public:
        TIMTimestampStats();

        void process(TIMPacket const & tim);
    };
}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::TIMPacket );

///////////////////////////////hh.e////////////////////////////////////////
#include "TIMPacket.cci"
//#include "TIMPacket.ct"
//#include "TIMPacket.cti"
#endif
