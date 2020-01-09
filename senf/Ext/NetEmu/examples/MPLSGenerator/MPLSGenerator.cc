// $Id:$
//
// Copyright (C) 2007
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
//     Thorsten Horstmann <tho@berlios.de>


// Definition of non-inline non-template functions

//#include "MCSniffer.hh"
//#include "MCSniffer.ih"

// Custom includes
#include <fstream>
#include <string>
#include <iomanip>
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/Packets/DataPacket.hh>
#include <senf/PPI/SocketSink.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>

#include <senf/Socket/Protocols/Raw/PacketSocketHandle.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>
#include <senf/Socket/NetdeviceController.hh>

#include "Configuration.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


class MPLSGenerator : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(MPLSGenerator);

    void timeout() {
        output( eth.clone());
        count_++;
        if (numPackets_ > 0 && (count_ > numPackets_)) {
            exit(0);
        }
        next_ += interval_;
        timer.timeout(next_);
    }
     
public:
    senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;
    senf::scheduler::TimerEvent timer;
    senf::ClockService::clock_type next_;
    senf::ClockService::clock_type interval_;
    senf::EthernetPacket eth;
    boost::uint64_t count_;
    boost::uint64_t numPackets_;

    MPLSGenerator( senf::MACAddress const & mac, Configuration const & config) :
        timer("mpls gen", boost::bind(&MPLSGenerator::timeout, this)),
        next_(senf::scheduler::now()),
        eth(senf::EthernetPacket::create()),
        count_(0),
        numPackets_(0)
    {
        noroute(output);

        eth->source()      << mac;
        eth->destination() << config.destination;
        eth->type_length() << 0x8847;
        senf::DataPacket data(senf::DataPacket::createAfter(eth, config.pktSize - 14));
        eth.finalizeAll();

        interval_ = senf::ClockService::microseconds( 1000000 / config.pktsPerSecond);
        numPackets_ = config.numPackets;

        SENF_LOG((senf::log::IMPORTANT) ("Starting generator on iface " << config.interface << " with MAC " << mac
                                         << ", destination " << config.destination 
                                         << ", interval " << senf::ClockService::in_microseconds(interval_) << " us, pktSize " << config.pktSize 
                                         << ", numPackets " << config.numPackets));

        // start the transmission
        timeout();
    }
};


int main(int argc, char const * argv[])
{
    Configuration configuration;

    if (!configuration.parse( argc, argv)) {
        exit(1);
    }

    senf::NetdeviceController netdevCtrl (configuration.interface);
    netdevCtrl.up();
    senf::MACAddress macAddr( netdevCtrl.hardwareAddress());
    
    senf::PacketSocketHandle socket;
    socket.bind(senf::LLSocketAddress(configuration.interface));
    senf::ppi::module::PassiveSocketSink<senf::ppi::TargetLLSocketWriter> sink(socket);
    MPLSGenerator generator( macAddr, configuration);

    senf::ppi::connect( generator, sink);
    senf::ppi::run();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MCSniffer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
