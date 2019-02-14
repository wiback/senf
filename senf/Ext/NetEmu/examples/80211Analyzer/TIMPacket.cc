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
{
    reset();
}

prefix_ senf::TIMSeqNoStats::TIMSeqNoStats(TIMPacket const & tim, bool llSeq)
{
    reset();

    if (llSeq) {
        processLLSeqNo(tim);
    } else {
        processSeqNo(tim);
    }
}

prefix_ void senf::TIMSeqNoStats::clear()
{
    goodTotal += good;
    goodBytesTotal += goodBytes;
    good = goodBytes = duplicate = late = lost = 0;
}

prefix_ void senf::TIMSeqNoStats::reset()
{
    clear();
    resyncs = 0;
    last_ = 0xFFFFFFFF;
    goodTotal = 0;
    goodBytesTotal = 0;
}

prefix_ void senf::TIMSeqNoStats::dump(std::ostream & os, senf::ClockService::clock_type const & period)
{
    os << "good " << good << ", goodBytes " << goodBytes;
    if (period) {
        os << ", good/s " << ((std::uint64_t(good) * 1000) / senf::ClockService::in_milliseconds(period));
        os << ", goodBytes/s " << ((std::uint64_t(goodBytes) * 1000) / senf::ClockService::in_milliseconds(period));
    }
    os << ", duplicate " << duplicate << ", late " << late << ", lost " << lost;
    os << ", resyncs " << resyncs;
}


///////////////////////////////////////////////////////////////////////////
// senf::TIMTimestampStats

prefix_ senf::TIMTimestampStats::TIMTimestampStats()
{
    reset();
}

prefix_ void senf::TIMTimestampStats::clear()
{
    delay.clear();
    pdv.clear();
}

prefix_ void senf::TIMTimestampStats::reset()
{
    clear();
    lastPD_ = 0x7FFFFFFF;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
