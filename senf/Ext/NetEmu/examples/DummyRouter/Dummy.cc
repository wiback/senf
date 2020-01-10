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
