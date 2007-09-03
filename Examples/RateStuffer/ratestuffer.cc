// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ppitest non-inline non-template implementation */

//#include "ppitest.hh"
//#include "ppitest.ih"

// Custom includes
#include <senf/Socket/Protocols/INet.hh>
#include <senf/PPI.hh>

//#include "ppitest.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;

// ////////////////////////////////////////////////////////////////////////
// RateFilter

class RateFilter
    : public module::Module
{
    SENF_PPI_MODULE(RateFilter);
public:

    connector::ActiveInput input;
    connector::ActiveOutput output;

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
    registerEvent(&RateFilter::timeout, timer);
}

void RateFilter::timeout()
{
    output(input());
}

// ////////////////////////////////////////////////////////////////////////

class RateStuffer
{
    module::ThrottleBarrier barrier;
    module::PassiveQueue    queue;
    module::CloneSource     generator;
    module::PriorityJoin    join;
    RateFilter              rateFilter;

public:
    connector::PassiveInput & input;
    connector::ActiveOutput & output;

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
        
// ////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////

// Module setup:
//
// 'O'        = active connector
// '>' or '<' = input connector
//
//                   +----------------------------------------------------+
//                   | stuffer                                            |
//                   |                                                    |
// [ udpReader ] O-->:---> [ queue ] -->O [      ]                        |
//                   |                    [ join ] -->O [ rateFilter] O-->:O--> [ udpWriter ]
//                   | [ generator ] -->O [      ]                        |
//                   |                                                    |
//                   +----------------------------------------------------+

int main(int argc, char * argv[])
{
    senf::UDPv4ClientSocketHandle inputSocket(
        senf::INet4SocketAddress("0.0.0.0:44344"));

    senf::ConnectedUDPv4ClientSocketHandle outputSocket(
        senf::INet4SocketAddress("localhost:44345"));

    module::ActiveSocketReader<>  udpReader  ( inputSocket );
    RateStuffer                   stuffer    ( 1000000000ul, 
                                               senf::DataPacket::create(std::string("<idle>\n")),
                                               2u, 1u );
    module::PassiveSocketWriter<> udpWriter  ( outputSocket );

    ppi::connect( udpReader, stuffer   );
    ppi::connect( stuffer,   udpWriter );

    ppi::run();

    return 0;
}

///////////////////////////////cc.e////////////////////////////////////////
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
