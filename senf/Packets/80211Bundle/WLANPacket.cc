// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Christian Niephaus <cni@berlios.de>


// Definition of non-inline non-template functions

#include "WLANPacket.hh"
//#include "WLANPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    void dumpBase(senf::WLANPacketParser const & p, std::ostream & os)
    {
        os << "  version                 : " << unsigned(p.version()) <<"\n"
           << "  type                    : " << unsigned(p.type()) << "\n"
           << "  subtype                 : " << unsigned(p.subtype()) << "\n"
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
        os << "\n"
           << "  duration                : " << unsigned(p.duration()) << "\n";
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
    os << senf::fieldName("destination")           << p->destinationAddress() << "\n"
       << senf::fieldName("source")                << p->sourceAddress() << "\n"
       << senf::fieldName("bss id")                << p->bssid() << "\n"
       << senf::fieldName("sequence number")       << p->sequenceNumber() << "\n"
       << senf::fieldName("fragment number")       << p->fragmentNumber() << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::WLANPacket_CtrlFrameType

prefix_ void senf::WLANPacket_CtrlFrameType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Control Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("receiver")              << p->receiverAddress() << "\n";
    if (p->is_rts())
        os << senf::fieldName("source")                << p->sourceAddress() << "\n";
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
    case 0 :
    case 1 :
        return addr2();
    // TODO wds frames
    // case 3 : return addr4();
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
    os << "\n"
       << senf::fieldName("source")                    << p->sourceAddress();
    if (p->dsBits()==0 || p->dsBits()==1) os << " (transmitter)";
    os << "\n"
       << senf::fieldName("bss id")                    << p->bssid();
    if (p->dsBits()==1) os << " (receiver)";
    else if (p->dsBits()!=0) os << " (transmitter)";
    os << "\n"
       << senf::fieldName("sequence number")           << p->sequenceNumber() << "\n"
       << senf::fieldName("fragment number")           << p->fragmentNumber() << "\n";
    if (p->has_qosField())
        os << senf::fieldName("QOS data")              << p->qosField() << "\n";
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
