// $Id$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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
