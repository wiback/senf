// $Id$
//
// Copyright (C) 2007
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

        queue.qdisc(ppi::ThresholdQueueing(high,low));
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
