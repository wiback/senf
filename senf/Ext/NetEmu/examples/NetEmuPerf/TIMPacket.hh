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

    //
    // TIM SeqNo Stats Analyzer
    //
    struct TIMSeqNoStats {
        std::uint32_t processed;
        std::uint32_t good;
        std::uint32_t goodBytes;
        std::uint32_t duplicate;
        std::uint32_t late;
        std::uint32_t lost;
        std::uint32_t resyncs;
        std::uint32_t last;
        
        TIMSeqNoStats() {
            reset();
        };

        void clear() {
            processed = good = goodBytes = duplicate = late = lost = 0;
        };

        void reset() {
            clear();
            resyncs = 0;
            last = 0xFFFFFFFF;
        };

        bool process(std::uint32_t seqNo, std::uint32_t payloadSize) {
            processed++;
            if (SENF_UNLIKELY(last == 0xFFFFFFFF)) {
                last = seqNo;
                good = 1;
                goodBytes = payloadSize;
                return true;
            }

            int diff = TIMPacketType::seqNoDiff(seqNo, last);

            if (SENF_LIKELY(diff == 1)) {
                // no loss
                good++;
                goodBytes += payloadSize;
                last = seqNo;
                return true;
            }
            else if (diff == 0) {
                // duplicate
                duplicate++;
            }
            else if (diff < 0) {
                // late frame(s) => reordering
                late++;
                if (diff <= -128) {
                    // resync to next expected SeqNo
                    resyncs++;
                    last = seqNo;
                }
            } else {
                // frame loss
                lost += diff-1;
                last = seqNo;
            }
            return false;
        }

        bool processSeqNo(TIMPacket const & tim) {
            return process(tim->sequenceNumber(), tim.size() - TIMPacketType::parser::fixed_bytes);
        }

        bool processLLSeqNo(TIMPacket const & tim) {
            return process(tim->linkLocalSeqNo(), tim.size() - TIMPacketType::parser::fixed_bytes);
        }

        void dump(std::ostream & os, senf::ClockService::clock_type const & period = senf::ClockService::clock_type(0)) {
            os << "processed " << processed << ", good " << good << ", goodBytes " << goodBytes;
            if (period) {
                os << ", good/s " << ((std::uint64_t(good) * 1000) / senf::ClockService::in_milliseconds(period));
                os << ", goodBytes/s " << ((std::uint64_t(goodBytes) * 1000) / senf::ClockService::in_milliseconds(period));
            }
            os << ", duplicate " << duplicate << ", late " << late << ", lost " << lost;
            os << ", resyncs " << resyncs;
        }
    };
}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( wiback::TIMPacket );

///////////////////////////////hh.e////////////////////////////////////////
#include "TIMPacket.cci"
//#include "TIMPacket.ct"
//#include "TIMPacket.cti"
#endif
