// $Id:$
//
// Copyright (C) 2009
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
//       Mathias Kretschmer <mtk@berlios.de>

/** \file
    \brief EthernetAnnotator implementation */

// Custom includes
#include "EthernetAnnotator.hh"

#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>

#define prefix_ 

prefix_ senf::emu::EthernetAnnotator::EthernetAnnotator(bool rxMode, bool mmapMode, senf::MACAddress const & id)
    : id_ (id),
      pvid_(std::uint16_t(-1)),
      vlanMismatch_(0),
      annotate_(false),
      rxMode_(rxMode), mmapMode_(mmapMode)
{
    route(input, output).autoThrottling( false);
    input.throttlingDisc(senf::ppi::ThrottlingDiscipline::NONE);
    if (rxMode_) {
        if (mmapMode_) {
            input.onRequest(&EthernetAnnotator::requestRxMMAP);
        } else {
            input.onRequest(&EthernetAnnotator::requestRx);
        }
    }
    else {
        input.onRequest(&EthernetAnnotator::requestTx);
    }

    handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_dummy, this, std::placeholders::_1);
}

prefix_ void senf::emu::EthernetAnnotator::id(MACAddress const & id)
{
    id_ = id;
}

prefix_ senf::MACAddress const & senf::emu::EthernetAnnotator::id()
    const
{
    return id_;
}

prefix_ void senf::emu::EthernetAnnotator::annotate(bool a)
{
    annotate_ = a;
    if (rxMode_ and (pvid_ == std::uint16_t(-1))) {
        if (annotate_)
            handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_dummy_annotate, this, std::placeholders::_1);
        else
            handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_dummy, this, std::placeholders::_1);
    }
}

prefix_ bool senf::emu::EthernetAnnotator::annotate()
    const
{
    return annotate_;
}

prefix_ void senf::emu::EthernetAnnotator::promisc(bool p)
{
    if (rxMode_ and mmapMode_) {
        if (p)
            input.onRequest(&EthernetAnnotator::requestRxMMAPpromisc);
        else
            input.onRequest(&EthernetAnnotator::requestRxMMAP);
    }
}

prefix_ std::uint32_t senf::emu::EthernetAnnotator::vlanMismatch()
{
    std::uint32_t tmp (vlanMismatch_);
    vlanMismatch_ = 0;
    return tmp;
}

prefix_ void senf::emu::EthernetAnnotator::netemu_annotations(senf::EthernetPacket const & eth)
{    
    eth.annotation<annotations::Interface>().value = id_;
    {
        emu::annotations::Quality const & q (eth.annotation<emu::annotations::Quality>());
        q.rssi  = 127;            // for now, we report the maximum signal 'quality'
        q.noise = -128;           // this should be read out via ethtool commands (i.e. for fiber links)
        q.snr = 255;
        q.flags.frameLength = eth.size();
    }
}

prefix_ void senf::emu::EthernetAnnotator::requestRx()
{
    senf::EthernetPacket const & eth (input());

    netemu_annotations(eth);
    eth.annotation<annotations::Timestamp>().fromScheduler();

    handle_pkt(eth);
}

prefix_ void senf::emu::EthernetAnnotator::requestRxMMAP()
{
    senf::EthernetPacket const & eth (input());

    netemu_annotations(eth);
    eth.annotation<annotations::Timestamp>().fromQueueBuffer(*(eth.annotation<senf::ppi::QueueBufferAnnotation>().value));
    
    handle_pkt(eth);
}

prefix_ void senf::emu::EthernetAnnotator::requestRxMMAPpromisc()
{
    senf::EthernetPacket const & eth (input());

    netemu_annotations(eth);
    eth.annotation<annotations::Timestamp>().fromQueueBuffer(*(eth.annotation<senf::ppi::QueueBufferAnnotation>().value));

    // check, if the h/w has removed the VLAN tag...
    // this might happen if VLAN offloading is not configured/working properly !!!
    boost::optional<unsigned> vlanId (eth.annotation<senf::ppi::QueueBufferAnnotation>()->vlan());
    if (SENF_UNLIKELY(vlanId)) {
        if (pvid_ == std::uint16_t(-1) or pvid_ == (*vlanId & 0xfff)) {
            // if no PVID is specified, but the TAG back in...
            if (eth.annotation<senf::ppi::QueueBufferAnnotation>()->tpid() == EthVLanSPacketType::etherType) {
                EthVLanSPacket vlan (EthVLanSPacket::createInsertBefore(eth.next()));
                vlan->vlanId() << *vlanId;
                vlan->type_length() << eth->type_length();
                EthernetPacket(eth).finalizeTo(vlan);
                vlan.reparse();
            } else {
                EthVLanCPacket vlan (EthVLanCPacket::createInsertBefore(eth.next()));
                vlan->vlanId() << *vlanId;
                vlan->type_length() << eth->type_length();
                EthernetPacket(eth).finalizeTo(vlan);
                vlan.reparse();
            }
        }
    }
    
    handle_pkt(eth);
}


prefix_ void senf::emu::EthernetAnnotator::requestTx()
{
    senf::EthernetPacket const & eth (input());

    handle_pkt(eth);
}

prefix_ void senf::emu::EthernetAnnotator::insertTag(std::uint16_t pvid)
{
    pvid_ = pvid;
    handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_insert_tag, this, std::placeholders::_1);
}

prefix_ void senf::emu::EthernetAnnotator::removeTag(std::uint16_t pvid)
{
    pvid_ = pvid;
    handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_remove_tag, this, std::placeholders::_1);
}

prefix_ void senf::emu::EthernetAnnotator::clearTag()
{
    pvid_ = std::uint16_t(-1);
    handle_pkt = std::bind(&senf::emu::EthernetAnnotator::handle_pkt_dummy, this, std::placeholders::_1);
}

prefix_ void senf::emu::EthernetAnnotator::handle_pkt_dummy(senf::EthernetPacket const & eth)
{    
    output(eth);
}

prefix_ void senf::emu::EthernetAnnotator::handle_pkt_dummy_annotate(senf::EthernetPacket const & eth)
{    
    output(prependAnnotationsPacket(eth));
}

prefix_ void senf::emu::EthernetAnnotator::handle_pkt_remove_tag(senf::EthernetPacket const & eth)
{
    if (eth->type_length() == EthVLanCPacketType::etherType) {
        auto vlan (eth.find<EthVLanCPacket>(senf::nothrow));
        if (SENF_UNLIKELY(!vlan or (vlan->vlanId() != pvid_))) {
            vlanMismatch_++;
            return;
        }
        
        // remove VLAN TAG here
        std::uint16_t tl (vlan->type_length());
        ::memmove(
                  vlan.data().begin(),
                  vlan.data().begin() + senf::EthVLanPacketParser::fixed_bytes,
                  vlan.size() - senf::EthVLanPacketParser::fixed_bytes);

        EthernetPacket tmp(eth);
        tmp.data().resize( tmp.size() - senf::EthVLanPacketParser::fixed_bytes);
        tmp->type_length() = tl;
        tmp.reparse();
    } else if (eth->type_length() == EthVLanSPacketType::etherType) {
        // we do not support S-Tags for now
        vlanMismatch_++;
        return;
    } else {
        vlanMismatch_++;
        return;
    }
    
    if (SENF_UNLIKELY(annotate_))
        output(prependAnnotationsPacket(eth));
    else
        output(eth);
}

prefix_ void senf::emu::EthernetAnnotator::handle_pkt_insert_tag(senf::EthernetPacket const & eth)
{
    senf::EthVLanCPacket vlanC (eth.next<senf::EthVLanCPacket>(senf::nothrow));
    if (SENF_UNLIKELY(vlanC)) {
        vlanMismatch_++;
        return;
    }
    senf::EthVLanSPacket vlanS (eth.next<senf::EthVLanSPacket>(senf::nothrow));
    if (SENF_UNLIKELY(vlanS)) {
        vlanMismatch_++;
        return;
    }
    
    std::uint16_t tl (eth->type_length());
    senf::Packet pkt (eth.next(senf::nothrow));
    if (SENF_LIKELY(pkt)) {
        senf::EthVLanCPacket vtmp (senf::EthVLanCPacket::createInsertBefore(pkt));
        vtmp->vlanId() = pvid_;
        vtmp->type_length() = tl;
        EthernetPacket(eth).finalizeTo(vtmp);
    } else {
        senf::EthVLanCPacket vtmp (senf::EthVLanCPacket::createAfter(eth));
        vtmp->vlanId() = pvid_;
        vtmp->type_length() = tl;
        EthernetPacket(eth).finalizeTo(vtmp);
    }

    if (SENF_UNLIKELY(annotate_))
        output(prependAnnotationsPacket(eth));
    else
        output(eth);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
