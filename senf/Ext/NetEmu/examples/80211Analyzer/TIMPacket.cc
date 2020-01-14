//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include "TIMPacket.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::TIMPacket );

prefix_ void senf::TIMPacketType::dump(packet p, std::ostream & os)
{
    os << "TIM Header: \n"
       << "  linkLocal seqNo : "  << p->linkLocalSeqNo() << std::endl
       << "  sequence number : "  << p->sequenceNumber() << std::endl
       << "  timestamp : "        << p->timestamp() << std::endl
       << "  GPS synchronized : " << p->syn() << std::endl
       << "  Odd size flag : "    << p->oddSize() << std::endl
       << "  padding : "          << p->padding() << std::endl;
}


///////////////////////////////////////////////////////////////////////////
// senf::TIMSeqNoStats

prefix_ senf::TIMSeqNoStats::TIMSeqNoStats()
    : SequenceNumberStatistics(22, 64)
{
    reset();
}

prefix_ senf::TIMSeqNoStats::TIMSeqNoStats(TIMPacket const & tim, bool llSeq)
    : SequenceNumberStatistics(22, 32)
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
