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
    \brief ppitest non-inline non-template implementation */

//#include "ppitest.hh"
//#include "ppitest.ih"

// Custom includes
#include <senf/Socket/Protocols/INet.hh>
#include <senf/PPI.hh>

//#include "ppitest.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;

//-////////////////////////////////////////////////////////////////////////
// RateFilter

class RateFilter
    : public module::Module
{
    SENF_PPI_MODULE(RateFilter);
public:

    connector::ActiveInput<> input;
    connector::ActiveOutput<> output;

    RateFilter(senf::ClockService::clock_type interval);

private:
    void timeout();

    ppi::IntervalTimer timer;
};

RateFilter::RateFilter(senf::ClockService::clock_type interval)
    : timer(interval)
{
    route(input,timer);
    route(timer,output);
    registerEvent(timer, &RateFilter::timeout);
}

void RateFilter::timeout()
{
    output(input());
}

//-////////////////////////////////////////////////////////////////////////

class RateStuffer
{
    module::ThrottleBarrier barrier;
    module::PassiveQueue    queue;
    module::CloneSource     generator;
    module::PriorityJoin    join;
    RateFilter              rateFilter;

public:
    connector::PassiveInput<> & input;
    connector::ActiveOutput<> & output;

    RateStuffer(senf::ClockService::clock_type interval,
                senf::Packet packet,
                unsigned high = 1,
                unsigned low  = 0)
    :   barrier    (),
        queue      (),
        generator  ( packet ),
        join       (),
        rateFilter ( interval ),
        input      ( barrier.input ),
        output     ( rateFilter.output )
    {
        ppi::connect( barrier,    queue      );
        ppi::connect( queue,      join       );
        ppi::connect( generator,  join       );
        ppi::connect( join,       rateFilter );

        queue.throttlingDisc(ppi::ThresholdThrottling(high,low));
    }
};

//-////////////////////////////////////////////////////////////////////////
//-////////////////////////////////////////////////////////////////////////

// Module setup:
//
// 'O'        = active connector
// '>' or '<' = input connector
//
//                   +----------------------------------------------------+
//                   | stuffer                                            |
//                   |                                                    |
// [ udpSource ] O-->:---> [ queue ] -->O [      ]                        |
//                   |                    [ join ] -->O [ rateFilter] O-->:O--> [ udpSink ]
//                   | [ generator ] -->O [      ]                        |
//                   |                                                    |
//                   +----------------------------------------------------+

int main(int argc, char * argv[])
{
    senf::UDPv4ClientSocketHandle inputSocket(
        senf::INet4SocketAddress("0.0.0.0:44344"));

    senf::ConnectedUDPv4ClientSocketHandle outputSocket(
        senf::INet4SocketAddress("localhost:44345"));

    module::ActiveSocketSource<>  udpSource ( inputSocket );
    RateStuffer                   stuffer   ( senf::ClockService::seconds(1),
                                              senf::DataPacket::create(std::string("<idle>\n")),
                                              2u, 1u );
    module::PassiveSocketSink<>   udpSink   ( outputSocket );

    ppi::connect( udpSource, stuffer   );
    ppi::connect( stuffer,   udpSink );

    ppi::run();

    return 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ppitest.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// End:
