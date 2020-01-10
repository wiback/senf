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


#include "WLANPacket.hh"
//#include "WLANPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    void dumpBase(senf::WLANPacketParser const & p, std::ostream & os)
    {
        os << "  version                 : " << p.version() << std::endl
           << "  type                    : " << p.type()    << std::endl
           << "  subtype                 : " << p.subtype() << std::endl
           << "  flags                   :";
        if ( p.toDS()           ) os << " toDS";
        if ( p.fromDS()         ) os << " fromDS";
        if ( p.moreFrag()       ) os << " moreFrag";
        if ( p.retry()          ) os << " retry";
        if ( p.pwrMgt()         ) os << " pwrMgt";
        if ( p.moreData()       ) os << " moreData";
        if ( p.protectedFrame() ) os << " protected";
        if ( p.order()          ) os << " order";

        if ( !(p.toDS() || p.fromDS() || p.moreFrag() || p.retry() || p.pwrMgt() ||
               p.moreData() || p.protectedFrame() || p.order()) )
            os << " none";
        os << std::endl
           << "  duration                : " << p.duration() << std::endl;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::WLANPacket_MgtFrameParser

prefix_ void senf::WLANPacket_MgtFrameParser::sequenceNumber(boost::uint16_t sn)
{
    seqNumber_2() = 0u | (sn >> 4 ) ;
    seqNumber_1() = 0u | sn;
}

prefix_ void senf::WLANPacket_MgtFrameType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Management Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("destination")           << p->destinationAddress() << std::endl
       << senf::fieldName("source")                << p->sourceAddress()      << std::endl
       << senf::fieldName("bss id")                << p->bssid()              << std::endl
       << senf::fieldName("sequence number")       << p->sequenceNumber()     << std::endl
       << senf::fieldName("fragment number")       << p->fragmentNumber()     << std::endl;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::WLANPacket_CtrlFrameType

prefix_ void senf::WLANPacket_CtrlFrameType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Control Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("receiver")              << p->receiverAddress() << std::endl;
    if (p->is_rts())
        os << senf::fieldName("source")            << p->sourceAddress() << std::endl;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::WLANPacket_DataFrameParser

prefix_ void senf::WLANPacket_DataFrameParser::sequenceNumber(boost::uint16_t sn)
{
    seqNumber_2() = 0u | (sn >> 4 ) ;
    seqNumber_1() = 0u | sn;
}

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::destinationAddress()
    const
{
    switch (dsBits()) {
    case 0 :
    case 2 :
        return addr1();
    default:
        return addr3();
    }
}

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::sourceAddress()
    const
{
    switch (dsBits()) {
    case 0:
    case 1:
        return addr2();
    case 3:
    	return addr4();
    default:
        return addr3();
    }
}

prefix_ senf::MACAddressParser senf::WLANPacket_DataFrameParser::bssid()
    const
{
    switch (dsBits()) {
    case 0 :
        return addr3();
    case 1 :
        return addr1();
    default:
        return addr2();
    }
}

prefix_ void senf::WLANPacket_DataFrameType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Data Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("destination")               << p->destinationAddress();
    if (p->dsBits()==0 || p->dsBits()==2) os << " (receiver)";
    os << std::endl
       << senf::fieldName("source")                    << p->sourceAddress();
    if (p->dsBits()==0 || p->dsBits()==1) os << " (transmitter)";
    os << std::endl
       << senf::fieldName("bss id")                    << p->bssid();
    if (p->dsBits()==1) os << " (receiver)";
    else if (p->dsBits()!=0) os << " (transmitter)";
    os << std::endl
       << senf::fieldName("sequence number")           << p->sequenceNumber() << std::endl
       << senf::fieldName("fragment number")           << p->fragmentNumber() << std::endl;
    if (p->has_qosField())
        os << senf::fieldName("QOS data")              << p->qosField() << std::endl;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
