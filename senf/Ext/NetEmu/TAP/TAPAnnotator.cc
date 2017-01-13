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
    \brief TAPAnnotator implementation */

// Custom includes
#include "TAPAnnotator.hh"

#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>

#define prefix_ 

prefix_ senf::emu::TAPAnnotator::TAPAnnotator(senf::MACAddress const & id)
    : id_ (id),
      rawMode_(false),
      annotate_(false),
      pvid_(std::uint16_t(-1))
{
    route(input, output).autoThrottling(false);
    input.throttlingDisc(senf::ppi::ThrottlingDiscipline::NONE);
    input.onRequest(&TAPAnnotator::request);
}

prefix_ void senf::emu::TAPAnnotator::id(MACAddress const & id)
{
    id_ = id;
}

prefix_ senf::MACAddress const & senf::emu::TAPAnnotator::id()
    const
{
    return id_;
}

prefix_ void senf::emu::TAPAnnotator::rawMode(bool r, std::uint16_t pvid)
{
    rawMode_ = r;
    pvid_ = pvid;
}

prefix_ void senf::emu::TAPAnnotator::annotate(bool a)
{
    annotate_ = a;
}

prefix_ bool senf::emu::TAPAnnotator::annotate()
    const
{
    return annotate_;
}


prefix_ void senf::emu::TAPAnnotator::request()
{
    senf::EthernetPacket eth (input());

    eth.annotation<annotations::Interface>().value = id_;
    eth.annotation<annotations::Timestamp>().fromScheduler();

    {
        emu::annotations::Quality & q (eth.annotation<emu::annotations::Quality>());
        q.rssi  = 127;            // for now, we report the maximum signal 'quality'
        q.noise = -128;           // this should be read out via ethtool commands (i.e. for fiber links)
        q.snr = 255;
        q.flags.frameLength = eth.size();
    }
    
    if (pvid_ != std::uint16_t(-1)) {
        senf::EthVLanPacket vlan (eth.next<senf::EthVLanPacket>(senf::nothrow));
        if (SENF_UNLIKELY(vlan)) {
            return;
        }
        std::uint16_t tl (eth->type_length());
        senf::Packet const & pkt (eth.next(senf::nothrow));
        if (SENF_LIKELY(pkt)) {
            senf::EthVLanPacket vtmp (senf::EthVLanPacket::createInsertBefore(pkt));
            vtmp->vlanId() = pvid_;
            vtmp->type_length() = tl;
            eth.finalizeTo(vtmp);
        } else {
            senf::EthVLanPacket vtmp (senf::EthVLanPacket::createAfter(eth));
            vtmp->vlanId() = pvid_;
            vtmp->type_length() = tl;
            eth.finalizeTo(vtmp);
        }
    }
    
    if (SENF_UNLIKELY(rawMode_ & annotate_)) {
        output(prependAnnotationsPacket(eth));
    } else {
        output(eth);
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
