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


#include "ICMPv6TypePacket.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6ErrDestUnreachable       );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6ErrTooBig                );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6ErrTimeExceeded          );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6ErrParamProblem          );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6EchoRequest              );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::ICMPv6EchoReply                );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MLDv2ListenerQuery             );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::NDPRouterSolicitationMessage   );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::NDPRouterAdvertisementMessage  );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::NDPNeighborSolicitationMessage );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::NDPNeighborAdvertisementMessage);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::NDPRedirectMessage             );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MLDv2ListenerReport            );


prefix_ void senf::ICMPv6ErrDestUnreachableParser::setErrCode(int code)
{
    ICMPv6Packet icmpv6 (Packet().rfind<ICMPv6Packet>(senf::nothrow));
    icmpv6->code() = code;
}

prefix_ void senf::ICMPv6ErrParamProblemParser::setErrCode(int code)
{
    ICMPv6Packet icmpv6 (packet().rfind<ICMPv6Packet>(senf::nothrow));
    icmpv6->code() = code;
}

prefix_ void senf::ICMPv6ErrTimeExceededParser::setErrCode(int code)
{
    ICMPv6Packet icmpv6 (packet().rfind<ICMPv6Packet>(senf::nothrow));
    icmpv6->code() = code;
}

prefix_ void senf::ICMPv6EchoRequestType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Echo Request:\n"
       << senf::fieldName("Identifier")         << p->identifier() << "\n"
       << senf::fieldName("SequenceNumber")     << p->seqNr() << "\n";
}

prefix_ void senf::ICMPv6EchoReplyType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Echo Reply:\n"
       << senf::fieldName("Identifier")         << p->identifier() << "\n"
       << senf::fieldName("SequenceNumber")     << p->seqNr() << "\n";
}

prefix_ void senf::ICMPv6ErrDestUnreachableType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Destination Unreachable (no further fields available here)\n";
}

prefix_ void senf::ICMPv6ErrTooBigType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Packet Too Big:\n"
       << senf::fieldName("MTU")            << p->mtu() << "\n";
}

prefix_ void senf::ICMPv6ErrTimeExceededType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Time Exceeded:\n"
       << senf::fieldName("Unused(32Bit)")  << p->unused() << "\n";
}

prefix_ void senf::ICMPv6ErrParamProblemType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Error Parameter Problem:\n"
       << senf::fieldName("Pointer")        << p->pointer() << "\n";
}

prefix_ void senf::MLDv2ListenerQueryType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Multicast Listener Query:\n"
       << senf::fieldName("Max. ResponseCode")         << p->maxResponseCode() << "\n"
       << senf::fieldName("Reserved(16Bit)")           << p->reserved() << "\n"
       << senf::fieldName("Multicast Address")         << p->mcAddress() << "\n"
       << senf::fieldName("Reserver(4Bit)")            << p->resv() << "\n"
       << senf::fieldName("Suppress Router-Side Processing") << p->sFlag() << "\n"
       << senf::fieldName("Querier's Robustness Variable") << p->qrv() << "\n"
       << senf::fieldName("Querier's Query Interval Code") << p->qqic() << "\n"
       << "  Source Addresses:\n";
    parser::srcAddresses_t::container_type c (p->srcAddresses());
    parser::srcAddresses_t::container_type::iterator i (c.begin());
    for (unsigned int nr =1; i != c.end(); ++i, ++nr)
        os << "    " << nr << ".) " << *i << "\n";
    os << "\n";
}

prefix_ void senf::MLDv2ListenerReportType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Multicast Listener Report Message:\n"
       << senf::fieldName("Reserved")              << p->reserved()
       <<"\n  Multicast Address Records:\n";

    MLDv2ListenerReport::Parser::mcastAddrRecords_t::container_type cAddrR (p->mcastAddrRecords() );
    MLDv2ListenerReport::Parser::mcastAddrRecords_t::container_type::iterator iAddrR (cAddrR.begin() );
    for (; iAddrR != cAddrR.end(); ++iAddrR) {
        os << senf::fieldName("  Record Type")         << unsigned(iAddrR->recordType()) << "\n"
           << senf::fieldName("  Multicast Address")   << iAddrR->mcAddress() << "\n"
           << "    Source Addresses\n:";
        MLDv2AddressRecordParser::srcAddresses_t::container_type cSrcAddr (iAddrR->srcAddresses());
        MLDv2AddressRecordParser::srcAddresses_t::container_type::iterator iSrcAddr (cSrcAddr.begin());
        for (;iSrcAddr != cSrcAddr.end();++iSrcAddr)
            os << "      " << *iSrcAddr << "\n";
        os << "    Auxiliary Data:\n";
        MLDv2AddressRecordParser::auxData_t::container_type cAuxD ( iAddrR->auxData() );
        MLDv2AddressRecordParser::auxData_t::container_type::iterator iAuxD (cAuxD.begin() );
        for (;iAuxD != cAuxD.end(); ++iAuxD)
            os << "      " << *iAuxD << "\n";
    }
}

prefix_ void senf::NDPRouterSolicitationMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Router Solicitation Message:\n"
       << senf::fieldName("Reserved(32Bit)")           << unsigned(p->reserved()) << "\n";
    parser::options_t::container_type optC (p->options());
    parser::options_t::container_type::const_iterator listIter (optC.begin());
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
    parser::options_t::container_type optC (p->options());
    parser::options_t::container_type::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
      listIter->dump(os);
    }
}

prefix_ void senf::NDPNeighborSolicitationMessageType::dump(packet p, std::ostream & os)
{
    os << "ICMPv6 Neighbor Discovery Neighbor Solicitation Message:\n"
       << senf::fieldName("Reserved(32Bit)")          << unsigned(p->reserved()) << "\n"
       << senf::fieldName("Target Address")           << p->target() << "\n";
    parser::options_t::container_type optC (p->options());
    parser::options_t::container_type::const_iterator listIter (optC.begin());
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
    parser::options_t::container_type optC(p->options() );
    parser::options_t::container_type::const_iterator listIter (optC.begin());
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
    parser::options_t::container_type optC(p->options() );
    parser::options_t::container_type::const_iterator listIter (optC.begin());
    for (; listIter != optC.end(); ++listIter) {
        listIter->dump(os);
    }
}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
