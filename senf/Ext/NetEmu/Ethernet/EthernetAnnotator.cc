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

prefix_ senf::emu::EthernetAnnotator::EthernetAnnotator(senf::MACAddress const & id)
    : id_ (id),
      rawMode_(false)
{
    route(input, output).autoThrottling( false);
    input.throttlingDisc(senf::ppi::ThrottlingDiscipline::NONE);
    input.onRequest(&EthernetAnnotator::request);
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

prefix_ void senf::emu::EthernetAnnotator::rawMode(bool r)
{
    rawMode_ = r;
}

prefix_ void senf::emu::EthernetAnnotator::request()
{
    senf::EthernetPacket eth (input());

    eth.annotation<annotations::Interface>().value = id_;

    // set the rx timestamp. Careful: this assumes that we are using an MMAP source !
    eth.annotation<annotations::Timestamp>().fromQueueBuffer(*(eth.annotation<senf::ppi::QueueBufferAnnotation>().value));

    {
        emu::annotations::Quality & q (eth.annotation<emu::annotations::Quality>());
        q.rssi  = 127;            // for now, we report the maximum signal 'quality'
        q.noise = -128;           // this should be read out via ethtool commands (i.e. for fiber links)
        q.snr = 255;
        q.flags.frameLength = eth.size();
    }

    boost::optional<unsigned> vlanId (eth.annotation<senf::ppi::QueueBufferAnnotation>()->vlan());
    if (SENF_UNLIKELY(vlanId)) {
        EthVLanPacket vlan (EthVLanPacket::createInsertBefore(eth.next()));
        vlan->vlanId() << *vlanId;
        vlan->type_length() << eth->type_length();
        eth.finalizeTo(vlan);
        vlan.reparse();
    }

    if (SENF_UNLIKELY(rawMode_)) {
        output(prependAnnotaionsPacket(eth));
    } else {
        output(eth);
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
