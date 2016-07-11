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
    \brief PVIDInserter implementation */

// Custom includes
#include "PVIDInserter.hh"

#define prefix_ 

prefix_ senf::emu::PVIDInserter::PVIDInserter()
    : pvid_(std::uint16_t(-1))
{
    route(input, output).autoThrottling( false);
    input.throttlingDisc(senf::ppi::ThrottlingDiscipline::NONE);
    input.onRequest(&PVIDInserter::requestBypass);
}

prefix_ void senf::emu::PVIDInserter::rawMode(std::uint16_t pvid)
{
    pvid_ = pvid;

    if (pvid_ == std::uint16_t(-1))
        input.onRequest(&PVIDInserter::requestBypass);
    else
        input.onRequest(&PVIDInserter::requestInsert);
}

prefix_ void senf::emu::PVIDInserter::requestBypass()
{
    output(input());
}


prefix_ void senf::emu::PVIDInserter::requestInsert()
{
    senf::EthernetPacket eth (input());

    senf::EthVLanPacket vlan (eth.next<senf::EthVLanPacket>(senf::nothrow));
    if (vlan) {
        // drop already tagged frames
        return;
    }

    std::uint16_t tl (eth->type_length());
    senf::Packet pkt (eth.next(senf::nothrow));
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

    output(eth);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
