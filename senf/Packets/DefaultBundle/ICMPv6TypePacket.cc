// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// Definition of non-inline non-template functions

// Custom includes
#include <senf/Packets/Packets.hh>
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 1,   senf::ICMPv6ErrDestUnreachable       );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 2,   senf::ICMPv6ErrTooBig                );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 3,   senf::ICMPv6ErrTimeExceeded          );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 4,   senf::ICMPv6ErrParamProblem          );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 128, senf::ICMPv6EchoRequest              );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 129, senf::ICMPv6EchoReply                );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 130, senf::MLDv2ListenerQuery             );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 133, senf::NDPRouterSolicitationMessage   );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 134, senf::NDPRouterAdvertisementMessage  );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 135, senf::NDPNeighborSolicitationMessage );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 136, senf::NDPNeighborAdvertisementMessage);
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 137, senf::NDPRedirectMessage             );
    SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 143, senf::MLDv2ListenerReport            );
}

prefix_ void senf::ICMPv6EchoRequestType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Echo Request:\n"
       << senf::fieldName("Identifier")         << unsigned(p->identifier()) << "\n"
       << senf::fieldName("SequenceNumber")     << unsigned(p->seqNr() ) << "\n";
}

prefix_ void senf::ICMPv6EchoReplyType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Echo Reply:\n"
       << senf::fieldName("Identifier")         << unsigned(p->identifier()) << "\n"
       << senf::fieldName("SequenceNumber")     << unsigned(p->seqNr()) << "\n";
}

prefix_ void senf::ICMPv6ErrDestUnreachableType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Destination Unreachable (no further fields available here)\n";    
}

prefix_ void senf::ICMPv6ErrTooBigType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Packet Too Big:\n"
       << senf::fieldName("MTU")            << unsigned(p->mtu() ) << "\n";    
}

prefix_ void senf::ICMPv6ErrTimeExceededType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Time Exceeded:\n"
       << senf::fieldName("Unused(32Bit)")  << unsigned(p->unused() ) << "\n";    
}

prefix_ void senf::ICMPv6ErrParamProblemType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Parameter Problem:\n"
       << senf::fieldName("Pointer")        << unsigned(p->pointer() ) << "\n";    
}

prefix_ void senf::MLDv2ListenerQueryType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Multicast Listener Query:\n"
       << senf::fieldName("Max. ResponseCode")         << unsigned(p->maxResponseCode()) << "\n"
       << senf::fieldName("Reserved(16Bit)")           << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Multicast Address")         << p->mcAddress() << "\n"
       << senf::fieldName("Reserver(4Bit)")            << unsigned(p->resv()) << "\n"
       << senf::fieldName("Suppress Router-Side Processing") << unsigned(p->sFlag()) << "\n"
       << senf::fieldName("Querier's Robustness Variable") << unsigned(p->qrv()) << "\n"
       << senf::fieldName("Querier's Query Interval Code") << unsigned(p->qqic()) << "\n"
       << "  Source Addresses:\n";
    senf::MLDv2ListenerQuery::Parser::srcAddresses_t::container c (p->srcAddresses());
    senf::MLDv2ListenerQuery::Parser::srcAddresses_t::container::iterator i (c.begin());
    for (unsigned int nr =1; i != c.end(); ++i, ++nr)
        os << "    " << nr << ".) " << *i << "\n";    
    os << "\n";    
}

prefix_ void senf::MLDv2ListenerReportType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Multicast Listener Report Message:\n"
       << senf::fieldName("Reserved")              << unsigned(p->reserved() ) 
       <<"\n  Multicast Address Records:\n";
    
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container cAddrR (p->mcastAddrRecords() );
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container::iterator iAddrR (cAddrR.begin() );
    for (; iAddrR != cAddrR.end(); ++iAddrR) {
        os << senf::fieldName("  Record Type")         << unsigned(iAddrR->recordType()) << "\n"
           << senf::fieldName("  Multicast Address")   << iAddrR->mcAddress() << "\n"
           << "    Source Addresses\n:";
        senf::MLDv2AddressRecordParser::srcAddresses_t::container cSrcAddr (iAddrR->srcAddresses());
        senf::MLDv2AddressRecordParser::srcAddresses_t::container::iterator iSrcAddr (cSrcAddr.begin());
        for (;iSrcAddr != cSrcAddr.end();++iSrcAddr)
            os << "      " << *iSrcAddr << "\n";
        os << "    Auxiliary Data:\n";
        senf::MLDv2AddressRecordParser::auxData_t::container cAuxD ( iAddrR->auxData() );
        senf::MLDv2AddressRecordParser::auxData_t::container::iterator iAuxD (cAuxD.begin() );
        for (;iAuxD != cAuxD.end(); ++iAuxD)
            os << "      " << *iAuxD << "\n";
    }
}

prefix_ void senf::NDPRouterSolicitationMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Router Solicitation Message:\n"
       << senf::fieldName("Reserved(32Bit)")           << unsigned(p->reserved()) << "\n";
    senf::NDPRouterSolicitationMessage::Parser::options_t::container optC(p->options() );
    senf::NDPRouterSolicitationMessage::Parser::options_t::container::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
      listIter->dump(os);
    }
}

prefix_ void senf::NDPRouterAdvertisementMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Router Advertisement Message:\n"
       << senf::fieldName("Current Hop Limit")             << unsigned(p->curHopLimit()) << "\n"
       << senf::fieldName("Managed Address Configuration") << unsigned(p->m()) << "\n"
       << senf::fieldName("Other Configuration")           << unsigned(p->o()) << "\n"
       << senf::fieldName("Reserved(6Bit)")                << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Router Lifetime")               << unsigned(p->routerLifetime()) << "\n"
       << senf::fieldName("Reachable Time")                << unsigned(p->reachableTime()) << "\n"
       << senf::fieldName("Retrans Timer")                 << unsigned(p->retransTimer()) << "\n";
    senf::NDPRouterAdvertisementMessage::Parser::options_t::container optC(p->options() );
    senf::NDPRouterAdvertisementMessage::Parser::options_t::container::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
      listIter->dump(os);
    }
}

prefix_ void senf::NDPNeighborSolicitationMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Neighbor Solicitation Message:\n"
       << senf::fieldName("Reserved(32Bit)")          << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Target Address")           << p->target() << "\n";
    senf::NDPNeighborSolicitationMessage::Parser::options_t::container optC(p->options() );
    senf::NDPNeighborSolicitationMessage::Parser::options_t::container::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
        listIter->dump(os);
    }
}

prefix_ void senf::NDPNeighborAdvertisementMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Neighbor Advertisement Message:\n"
       << senf::fieldName("Router Flag")           << unsigned(p->r()) << "\n"
       << senf::fieldName("Solicited Flag")        << unsigned(p->s()) << "\n"
       << senf::fieldName("Override Flag")         << unsigned(p->o()) << "\n"
       << senf::fieldName("Reserved(29Bit)")       << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Target Address")        << p->target() << "\n";
    senf::NDPNeighborAdvertisementMessage::Parser::options_t::container optC(p->options() );
    senf::NDPNeighborAdvertisementMessage::Parser::options_t::container::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
       listIter->dump(os);
    }
}

prefix_ void senf::NDPRedirectMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Redirect Message:\n"
       << senf::fieldName("Reserved(32Bit)")       << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Target Address")        << p->target() << "\n"
       << senf::fieldName("Destination Address")   << p->destination() << "\n";
    senf::NDPRedirectMessage::Parser::options_t::container optC(p->options() );
    senf::NDPRedirectMessage::Parser::options_t::container::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
      listIter->dump(os);
    }
}
///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
