// $Id:$
//
// Copyright (C) 2009
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
    \brief Decoration non-inline non-template implementation */

#include "Decoration.hh"
//#include "Decoration.ih"

// Custom includes
#include "Config.hh"

//#include "Decoration.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// wiback::PacketCounter

prefix_ wiback::PacketCounter::PacketCounter()
    : count_ (0u)
{}

prefix_ unsigned wiback::PacketCounter::count()
    const
{
    return count_;
}

prefix_ void wiback::PacketCounter::v_handlePacket(senf::Packet const & p)
{
    ++ count_;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// wiback::InterfaceAnnotater

prefix_ wiback::InterfaceAnnotater::InterfaceAnnotater(senf::emu::Interface & interface)
    : interface_ (interface)
{
    route(input, output);
    input.onRequest(&InterfaceAnnotater::request);
}

prefix_ void wiback::InterfaceAnnotater::request()
{
    senf::Packet p (input());
    p.annotation<senf::emu::annotations::Interface>().value = interface_.id();
    output(p);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// wiback::ReceiveGroup

prefix_ wiback::ReceiveGroup::ReceiveGroup(senf::emu::Interface & interface)
    : annotator (interface), input (counter.input), output (annotator.output)
{
    senf::ppi::connect(counter, rateAnalyzer);
    senf::ppi::connect(rateAnalyzer, annotator);

    interface.startStatistics.connect(senf::membind(&ReceiveGroup::startStatistics, this));

    interface.registerStatistics("in-packetsPerSecond", rateAnalyzer.signals.packetsPerSecond);
    interface.registerStatistics("in-bytesPerSecond", rateAnalyzer.signals.bytesPerSecond);
    interface.registerStatistics("in-bytesPerPacket", rateAnalyzer.signals.bytesPerPacket);
}

prefix_ void wiback::ReceiveGroup::init(Config & config)
{}

prefix_ void wiback::ReceiveGroup::startStatistics(senf::ClockService::clock_type pollInterval)
{
    rateAnalyzer.startStatistics(pollInterval);
}

prefix_ unsigned wiback::ReceiveGroup::receivedPackets()
    const
{
    return counter.count();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// wiback::TransmitGroup

prefix_ wiback::TransmitGroup::TransmitGroup(senf::emu::Interface & interface)
    : input(counter.input), output(rateAnalyzer.output), peer(0xfffffffffeffffffull)
{
    senf::ppi::connect(counter, rateAnalyzer);

    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;

    interface.startStatistics.connect(senf::membind(&TransmitGroup::startStatistics, this));

    interface.registerStatistics("out-packetsPerSecond", rateAnalyzer.signals.packetsPerSecond);
    interface.registerStatistics("out-bytesPerSecond", rateAnalyzer.signals.bytesPerSecond);
    interface.registerStatistics("out-bytesPerPacket", rateAnalyzer.signals.bytesPerPacket);

    interface.consoleDir()
        .add("peer", fty::Variable(boost::ref(peer)))
        .doc("peer of this interface, defaults to FF:FF:FF:FF-FE:FF:FF:FF (broadcast)");
}

prefix_ void wiback::TransmitGroup::init(Config & config)
{}

prefix_ void wiback::TransmitGroup::startStatistics(senf::ClockService::clock_type pollInterval)
{
    rateAnalyzer.startStatistics(pollInterval);
}

prefix_ unsigned wiback::TransmitGroup::sentPackets()
    const
{
    return counter.count();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Decoration.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
