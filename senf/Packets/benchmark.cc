//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
