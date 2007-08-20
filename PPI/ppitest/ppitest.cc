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
#include "Socket/Protocols/INet/UDPSocketHandle.hh"
#include "Socket/Protocols/INet/ConnectedUDPSocketHandle.hh"
#include "Socket/Protocols/INet/INetAddressing.hh"
#include "PPI/SocketReader.hh"
#include "PPI/SocketWriter.hh"
#include "PPI/Module.hh"
#include "PPI/IntervalTimer.hh"
#include "PPI/Joins.hh"
#include "PPI/PassiveQueue.hh"
#include "PPI/Setup.hh"

//#include "ppitest.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;

namespace {

    class RateFilter
        : public module::Module
    {
        SENF_PPI_MODULE(RateFilter);
    public:

        connector::ActiveInput input;
        connector::ActiveOutput output;

        RateFilter(senf::ClockService::clock_type interval) : timer(interval) {
            route(input,output);
            route(input,timer);
            registerEvent(&RateFilter::timeout, timer);
        }

    private:
        void timeout() {
            output(input());
        }

        ppi::IntervalTimer timer;
    };

    class CopyPacketGenerator
        : public module::Module
    {
        SENF_PPI_MODULE(CopyPacketGenerator);
    public:

        connector::PassiveOutput output;

        CopyPacketGenerator(senf::Packet p) : packet(p) {
            noroute(output);
            output.onRequest(&CopyPacketGenerator::request);
        }

    private:
        void request() {
            output(packet);
        }

        senf::Packet packet;
    };
}

// Module setup:
//
// 'O'        = active connector
// '>' or '<' = input connector
//
// [ udpReader ] O--> [ queue ] -->O [      ]
//                                   [ join ] -->O [ rateFilter] O--> [ udpWriter ]
//                [ generator ] -->O [      ]

int main(int argc, char * argv[])
{
    senf::UDPv4ClientSocketHandle inputSocket;
    inputSocket.bind(senf::INet4SocketAddress("0.0.0.0:44344"));

    senf::ConnectedUDPv4ClientSocketHandle outputSocket(
        senf::INet4SocketAddress("localhost:44345"));

    module::ActiveSocketReader<>  udpReader  (inputSocket);
    module::PassiveQueue          queue;
    CopyPacketGenerator           generator  (senf::DataPacket::create(std::string("<idle>\n")));
    module::PriorityJoin          join;
    RateFilter                    rateFilter (1000000000ul);
    module::PassiveSocketWriter<> udpWriter  (outputSocket);

    ppi::connect( udpReader,  queue      );
    ppi::connect( queue,      join       );
    ppi::connect( generator,  join       );
    ppi::connect( join,       rateFilter );
    ppi::connect( rateFilter, udpWriter  );

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
