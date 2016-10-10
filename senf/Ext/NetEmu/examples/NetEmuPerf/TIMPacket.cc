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

SENF_PACKET_INSTANTIATE_TEMPLATE( wiback::TIMPacket );

prefix_ void wiback::TIMPacketType::dump(packet p, std::ostream & os)
{
    os << "TIM Header: \n"
       << "  linkLocal seqNo : " << p->linkLocalSeqNo() << std::endl
       << "  sequence number : " << p->sequenceNumber() << std::endl
       << "  timestamp : " << p->timestamp() << std::endl
       << "  GPS synchronized : " << p->syn() << std::endl
       << "  Odd size flag : " << p->oddSize() << std::endl;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
