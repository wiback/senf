// $Id:$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Dummy non-inline non-template implementation */

#include "Dummy.hh"
//#include "Dummy.ih"

// Custom includes
#include <senf/Ext/NetEmu/InterfaceAPI.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/EmulatedInterface.hh>

//#include "Dummy.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ wiback::DummyRouter::DummyRouter(Config & config, Setup<CARMENDecoration> & setup)
    : config_ (config), setup_ (setup), timer_ (config_.probeInterval()),
      random_ (0, setup_.interfaces().size()-1), generator_ (), delayedTicks_ (0u),
      maxTickDelay_ (0u)
{
    counter_.value = 0u;
    noroute(input);
    route(timer_, output);
    input.onRequest(&DummyRouter::request);
    registerEvent(timer_, &DummyRouter::tick);
    timer_.enabled(false);
}

namespace {

    unsigned x (1);

}

prefix_ void wiback::DummyRouter::request()
{
    senf::EthernetPacket eth (input());

    if (eth->type_length() != 0xffff)
        return;

    for (unsigned i (config_.burnCycles()); i > 0; --i)
        x += x&i;

    SENF_LOG(("INCOMING ON " << senf::emu::Interface::nodeId() << "\n"
              << senf::emu::dumpPacketInfo(eth)));
}

prefix_ void wiback::DummyRouter::tick()
{
    senf::ClockService::clock_type delay (now() - time());
    if (delay > maxTickDelay_)
        maxTickDelay_ = delay;
    if (delay > config_.maxSchedulingDelay()) {
        SENF_LOG((senf::log::IMPORTANT)("Excessive scheduling delay of "
                                        << senf::ClockService::reltime(delay) << " detected."));
        ++ delayedTicks_;
    }

    for (unsigned n (config_.numPackets()); n > 0; --n)
        sendPacket();
}

prefix_ void wiback::DummyRouter::sendPacket()
{
    senf::EthernetPacket eth (senf::EthernetPacket::create());
    senf::Packet p (senf::DataPacket::createAfter(eth, counter_.bytes));
    ++ counter_.value;

    senf::emu::Interface::Decorator & outif (
        *(setup_.interfaces().begin()+random_(generator_)));
    eth.annotation<senf::emu::annotations::Interface>().value = outif.interface().id();
    eth->type_length() = 0xFFFF;
    eth->source() = outif.interface().id();
    eth->destination() = CARMENDecoration::transmitModule(outif).peer;

    eth.finalizeAll();

    SENF_LOG(("OUTGOING ON " << senf::emu::Interface::nodeId() << "\n"
              << senf::emu::dumpPacketInfo(eth)));

    output(eth);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Dummy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
