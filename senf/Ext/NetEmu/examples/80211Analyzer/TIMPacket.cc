//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//

#include "TIMPacket.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::TIMPacket );

prefix_ void senf::TIMPacketType::dump(packet p, std::ostream & os)
{
    os << "TIM Header: \n"
       << "  linkLocal seqNo : " << p->linkLocalSeqNo() << std::endl
       << "  sequence number : " << p->sequenceNumber() << std::endl
       << "  timestamp : " << p->timestamp() << std::endl
       << "  GPS synchronized : " << p->syn() << std::endl
       << "  Odd size flag : " << p->oddSize() << std::endl
       << "  padding : " << p->padding() << std::endl;
}


///////////////////////////////////////////////////////////////////////////
// senf::TIMSeqNoStats

prefix_ senf::TIMSeqNoStats::TIMSeqNoStats()
    : SequenceNumberStatistics(TIMPacketParser::sequenceNumber_t::max_value + 1, (TIMPacketParser::sequenceNumber_t::max_value + 1) / 10, 64)
{
    reset();
}

prefix_ senf::TIMSeqNoStats::TIMSeqNoStats(TIMPacket const & tim, bool llSeq)
    : SequenceNumberStatistics(TIMPacketParser::sequenceNumber_t::max_value + 1, (TIMPacketParser::sequenceNumber_t::max_value + 1) / 10, 64)
{
    reset();

    if (llSeq) {
        processLLSeqNo(tim);
    } else {
        processSeqNo(tim);
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::TIMTimestampStats

prefix_ senf::TIMTimestampStats::TIMTimestampStats()
    : TimestampStatistics(14)
{
    reset();
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
