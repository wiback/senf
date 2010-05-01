// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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

#include "WLANPacket.hh"
//#include "WLANPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p///////////////////////////////////////

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

prefix_ void senf::WLANPacket_MgtFrameParser::sequenceNumber(boost::uint16_t sn)
{
    seqNumber_2() = 0u | (sn >> 4 ) ;
    seqNumber_1() = 0u | sn;
}

prefix_ void senf::WLANPacket_MgtFrameType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Mangement Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("destination")           << p->destinationAddress() << "\n"
       << senf::fieldName("source")                << p->sourceAddress() << "\n"
       << senf::fieldName("bss id")                << p->bssid() << "\n"
       << senf::fieldName("sequence number")       << p->sequenceNumber() << "\n"
       << senf::fieldName("fragment number")       << p->fragmentNumber() << "\n";
}

prefix_ void senf::WLANPacket_CtrlFrameType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "802.11 MAC Control Frame:\n";
    dumpBase(p.parser(), os);
    os << senf::fieldName("receiver")              << p->receiverAddress() << "\n";
    if (p->is_rts())
        os << senf::fieldName("source")                << p->sourceAddress() << "\n";
}

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

prefix_ void senf::WLANPacket_DataFrameType::dump(packet p, std::ostream &os)
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

///////////////////////////////cc.e////////////////////////////////////////
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
