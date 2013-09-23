// $Id$
//
// Copyright (C) 2012
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief benchmark non-inline non-template implementation */

#include "Packets.hh"
//#include "benchmark.ih"

// Custom includes
#include <iostream>
#include <senf/PPI.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Utils/Console.hh>

//#include "benchmark.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class BenchmarkModule
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(BenchmarkModule);
    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        BenchmarkModule()
            {
                route(input, output);
                input.onRequest(& BenchmarkModule::onRequest);
            }

    private:
        void onRequest()
            {
                output(senf::EthernetPacket::createBefore(input.read()));
            }
    };

    void displayRate(float rate)
    {
        std::cerr << rate << std::endl;
    }

}

int main(int argc, char const * argv[])
{
    unsigned n (atoi(argv[1]));
    senf::EthernetPacket packet (senf::EthernetPacket::create());
    senf::ppi::module::debug::ActiveSource source;
    //senf::ppi::module::CloneSource source (packet);
    //senf::ppi::module::ActiveFeeder feeder;
    BenchmarkModule first;
    //senf::ppi::module::RateAnalyzer analyzer;
    senf::ppi::module::DiscardSink sink;

    senf::ppi::connect(source, first);

    BenchmarkModule * previous (&first);
    for (unsigned i (0); i < 4; ++i) {
        BenchmarkModule * next (new BenchmarkModule);
        senf::ppi::connect(*previous, *next);
        previous = next;
    }

    senf::ppi::connect(*previous, sink);

    //analyzer.signals.packetsPerSecond.connect(displayRate);
    //analyzer.startStatistics(senf::ClockService::seconds(1));

    //senf::console::Server::start(senf::INet4SocketAddress(23232u));

    //senf::ppi::run();

    while (true) {
        senf::ClockService::clock_type start (senf::ClockService::now());
        for (unsigned i (0); i < n; ++i)
            source.submit(packet.clone());
        senf::ClockService::int64_type msecs (
            senf::ClockService::in_milliseconds(senf::ClockService::now() - start));
        std::cout << (1000 * n) / msecs << std::endl;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "benchmark.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u benchmark"
// End:
