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
