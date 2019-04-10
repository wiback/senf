// $Id: AnnotationsPacket.cc 2155 2015-04-11 19:17:44Z mtk $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief AnnotationsPacket non-inline non-template implementation */

#include "AnnotationsPacket.hh"
//#include "AnnotationsPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Utils/Format.hh>
#include <senf/Utils/senflikely.hh>
#include <senf/Packets/PacketTypes.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include "Annotations.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::AnnotationsPacket );

SENF_PACKET_REGISTRY_REGISTER( senf::EtherOUIExtTypes,
        senf::EtherOUIExtTypes::type(0x001113u, senf::AnnotationsPacketType::extType), senf::AnnotationsPacket);

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::AnnotationsPacketType

prefix_ void senf::AnnotationsPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << senf::fieldName("interfaceId")  << p->type().value()                          << std::endl;
    os << senf::fieldName("timestampMAC") << format::dumpint(p->timestampMAC().value()) << std::endl;
    os << senf::fieldName("timestamp")    << format::dumpint(p->timestamp().value())    << std::endl;
    os << senf::fieldName("modulationId") << format::dumpint(p->modulationId().value()) << std::endl;
    os << senf::fieldName("snr")          << format::dumpint(p->snr().value())          << std::endl;
    os << senf::fieldName("rssi")         << format::dumpint(p->rssi().value())         << std::endl;
    os << senf::fieldName("airTime")      << format::dumpint(p->airTime().value())      << std::endl;
    os << senf::fieldName("type")         << format::dumpint(p->type().value())         << std::endl;

    os << senf::fieldName("flags.corrupt")       << format::dumpint(p->corrupt().value())       << std::endl;
    os << senf::fieldName("flags.retransmitted") << format::dumpint(p->retransmitted().value()) << std::endl;
    os << senf::fieldName("flags.duplicated")    << format::dumpint(p->duplicated().value())    << std::endl;
    os << senf::fieldName("flags.reordered")     << format::dumpint(p->reordered().value())     << std::endl;
    os << senf::fieldName("flags.aggregated")    << format::dumpint(p->aggregated().value())    << std::endl;
    os << senf::fieldName("flags.gap")           << format::dumpint(p->gap().value())           << std::endl;
    os << senf::fieldName("flags.length")        << format::dumpint(p->length().value())        << std::endl;
}

prefix_ senf::PacketInterpreterBase::factory_t senf::AnnotationsPacketType::nextPacketType(packet p)
{
    static factory_t typeFactory[] = { WLANPacket_MgtFrame::factory(),
                                       WLANPacket_CtrlFrame::factory(),
                                       WLANPacket_DataFrame::factory(),
                                       EthernetPacket::factory(),
                                       DataPacket::factory(),
                                       no_factory() };

    return typeFactory[p->type()];
}

prefix_ void senf::AnnotationsPacketType::finalize(packet p)
{
    Packet n (p.next(nothrow));
    if (SENF_LIKELY(n)) {
        if      (n.is<WLANPacket_MgtFrame>())
            p->type() << 0;
        else if (n.is<WLANPacket_CtrlFrame>())
            p->type() << 1;
        else if (n.is<WLANPacket_DataFrame>())
            p->type() << 2;
        else if (n.is<EthernetPacket>())
            p->type() << 3;
        else if (n.is<DataPacket>())
            p->type() << 4;
        else
            p->type() << 0xff;
    }
    // Do NOT reset type_length if the type is not known ... doing this will destroy read packets
}

prefix_ senf::EthernetPacket senf::prependAnnotationsPacket(Packet const & pkt, MACAddress const & src_, MACAddress const & dst_)
{
    AnnotationsPacket const & ap (AnnotationsPacket::createBefore(pkt));
    EthOUIExtensionPacket const & oui (EthOUIExtensionPacket::createBefore(ap));
    EthernetPacket eth (EthernetPacket::createBefore(oui));

    senf::MACAddress src (senf::MACAddress::None);
    senf::MACAddress dst (senf::MACAddress::Broadcast);
    if (SENF_LIKELY(pkt.is<senf::EthernetPacket>())) {
        src = pkt.as<senf::EthernetPacket>()->source();
        dst = pkt.as<senf::EthernetPacket>()->destination();
    }
    else if (pkt.is<senf::WLANPacket_MgtFrame>()) {
        src = pkt.as<senf::WLANPacket_MgtFrame>()->sourceAddress();
    }
    else if (pkt.is<senf::WLANPacket_CtrlFrame>() and pkt.as<senf::WLANPacket_CtrlFrame>()->is_rts()) {
        src = pkt.as<senf::WLANPacket_CtrlFrame>()->sourceAddress();
    }
    else if (pkt.is<senf::WLANPacket_DataFrame>()) {
        src = pkt.as<senf::WLANPacket_DataFrame>()->sourceAddress();
        dst = pkt.as<senf::WLANPacket_DataFrame>()->destinationAddress();
    }
    else if (pkt.is<senf::DataPacket>()) {
        src = src_;
        dst = dst_;
    }

    eth->source()       << src;
    eth->destination()  << (dst.multicast() ? pkt.annotation<emu::annotations::Interface>().value : dst);

    ap->interfaceId()   << pkt.annotation<emu::annotations::Interface>().value;
    ap->timestampMAC()  << senf::ClockService::in_nanoseconds(pkt.annotation<emu::annotations::Timestamp>().as_clock_type());
    ap->timestamp()     << senf::ClockService::in_nanoseconds(pkt.annotation<emu::annotations::Timestamp>().as_clock_type());
    ap->modulationId()  << pkt.annotation<emu::annotations::WirelessModulation>().id;
    ap->snr()           << pkt.annotation<emu::annotations::Quality>().snr;
    ap->rssi()          << pkt.annotation<emu::annotations::Quality>().rssi;
    ap->airTime()       << pkt.annotation<emu::annotations::Quality>().airTime;
    ap->corrupt()       << pkt.annotation<emu::annotations::Quality>().flags.frameCorrupt;
    ap->retransmitted() << pkt.annotation<emu::annotations::Quality>().flags.frameRetransmitted;
    ap->duplicated()    << pkt.annotation<emu::annotations::Quality>().flags.frameDuplicate;
    ap->reordered()     << pkt.annotation<emu::annotations::Quality>().flags.frameReordered;
    ap->aggregated()    << pkt.annotation<emu::annotations::Quality>().flags.frameAggregated;
    ap->gap()           << pkt.annotation<emu::annotations::Quality>().flags.framePredecessorLost;
    ap->length()        << pkt.annotation<emu::annotations::Quality>().flags.frameLength;
    
    eth.finalizeTo(ap);
    return eth;
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
