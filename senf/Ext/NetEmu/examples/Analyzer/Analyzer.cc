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


#include <linux/net_tstamp.h>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DataPacket.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/StatisticAccumulator.hh>
#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/PPI/QueueSocketSourceSink.hh>
#include <senf/PPI/PPI.hh>
#include <senf/Socket/NetdeviceController.hh>

#include <senf/Ext/NetEmu/Annotations.hh>

#include "Configuration.hh"
#include "InternalThroughputTestPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


class Generator : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Generator);

public:
    senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;
    senf::scheduler::TimerEvent timer;
    senf::EthernetPacket eth;
    unsigned numPkts_;
    unsigned finPkts_;

    Generator(senf::MACAddress const & mac, Configuration const & config) :
        timer("trigger", boost::bind(&Generator::sendPkts, this)),
        eth(senf::EthernetPacket::create()),
        numPkts_(config.numPackets),
        finPkts_(0)
    {
        noroute(output);
        
        eth->source()      << mac;
        eth->destination() << config.destination;
        auto ouiExt (senf::EthOUIExtensionPacket::createAfter(eth));
        auto testPkt (emu::InternalThroughputTestPacket::createAfter(ouiExt));
        senf::DataPacket data(senf::DataPacket::createAfter(testPkt, config.pktSize - eth.size()));

        testPkt->sessionId() << config.sessionId;
        testPkt->numPkts()   << numPkts_;
        testPkt->magic()     << 0xaffe;

        eth.finalizeAll();

        SENF_LOG((senf::log::IMPORTANT) ("Starting generator on iface " << config.interface << " with MAC " << mac
                                         << ", destination " << config.destination 
                                         << ", pktSize " << config.pktSize 
                                         << ", numPackets " << config.numPackets
                                         << ", sessionId " << config.sessionId));

        // trigger sendPkts() in 1s
        timer.timeout(senf::scheduler::now() + senf::ClockService::seconds(1));
    }

    void sendPkts() {
        // push packets into MMAP buffer
        auto testPkt (eth.find<emu::InternalThroughputTestPacket>());
        testPkt->timestamp() << senf::ClockService::in_nanoseconds(senf::scheduler::now());
        for (unsigned n = 0; n < numPkts_; n++) {
            testPkt->seqNo() << n;
            output(eth.clone());
        }

        // FIN sequence
        std::cout << "FIN Sequence..." << std::endl;
        timer.action(boost::bind(&Generator::FIN, this));
        timer.timeout(senf::scheduler::now() + senf::ClockService::milliseconds(10));
        // flush/send will be triggered as soon as we exit this method
    }

    void FIN() {
	if (finPkts_ < 10) {
          auto testPkt (eth.find<emu::InternalThroughputTestPacket>());
          testPkt->seqNo() << numPkts_;
          output(eth.clone());
          timer.timeout(senf::scheduler::now() + senf::ClockService::milliseconds(10));
          finPkts_++;
        }
        else {
          // terminate us in 1s
          timer.action(boost::bind(&Generator::terminate, this));
          timer.timeout(senf::scheduler::now() + senf::ClockService::seconds(1));
        }
    }

    void terminate() {
        senf::scheduler::terminate();
    }
};

class Analyzer : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Analyzer);

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
    senf::MACAddress ourMAC_;
    std::map<std::uint32_t,std::int64_t> tstamps;
    senf::StatisticAccumulator<float> ThroughputDiffs;
    unsigned sessionId_;
    bool verbose_;
    unsigned OLd_numPkts=50;

    Analyzer(senf::MACAddress const & mac, Configuration const & config) :
        ourMAC_(mac),
        sessionId_(0),
        verbose_(config.verbose)
    {
        noroute(input);
        input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
        input.onRequest( &Analyzer::request);

        SENF_LOG((senf::log::IMPORTANT) ("Starting analyzer on iface " << config.interface << " with MAC " << mac) );
    }

    void request() {
        senf::EthernetPacket const & eth (input());

        // fetch the rx timestamp from the meta data
        eth.annotation<senf::emu::annotations::Timestamp>().fromQueueBuffer(*(eth.annotation<senf::ppi::QueueBufferAnnotation>().value));

        if (eth->destination() == ourMAC_) {
            auto testPkt (eth.find<emu::InternalThroughputTestPacket>(senf::nothrow));
            if (testPkt) {
                if (sessionId_ == 0 and testPkt->seqNo() < testPkt->numPkts())
                    sessionId_ = testPkt->sessionId();
                if (sessionId_ != testPkt->sessionId()) {
                    if (testPkt->seqNo() != testPkt->numPkts()) {
                        SENF_LOG( (senf::log::IMPORTANT) ("Packet #" << testPkt->seqNo() << " ignored due to mismatching sessionId") );
                    }
                    return;
                }
 
                // we have a valid packet
                auto rxTstamp (senf::ClockService::in_nanoseconds(eth.annotation<senf::emu::annotations::Timestamp>().as_clock_type()));
                
                if (verbose_) {
                SENF_LOG( (senf::log::IMPORTANT) ("Packet #" << testPkt->seqNo() << "/" << testPkt->numPkts() << " received with length " << eth.size()
                                                  << ", timestamp " << rxTstamp) );
                }

                if (testPkt->seqNo() < testPkt->numPkts()) { 
                    tstamps.insert(std::make_pair(testPkt->seqNo(), rxTstamp));
                } else {
                    // Analysis of tstamp vector here
                    analyzeData(testPkt->numPkts(), eth.size());                        
                    // we are done
                    tstamps.clear();
                    sessionId_ = 0;
                }
            }
        }
    }

    //
    // Your code here !!!
    //
    
    void analyzeData(unsigned numPkts, unsigned pktSize) {
        if (tstamps.size() < 2) {
             std::cerr << "Insufficient data for evaluation" << std::endl;
             return;
        }
        std::map<std::int32_t,std::uint32_t> distribution;
        senf::StatisticAccumulator<std::int64_t> txTime;

        auto start (tstamps.begin());
        for (auto it (std::next(start)); it != tstamps.end(); it++, start++) {

            if (it->first - start->first == 1) { 
              std::cout << "(" << start->first << " " << it->second - start->second << ")";
              txTime.accumulate(it->second - start->second);
              distribution[(it->second - start->second)/1000]++;
            }
            else
              std::cout << "(loss after " << start->first << ", diff=" << it->first - start->first << ")";
        }

        std::cout << std::endl;
        
        std::cout << "Sample distribution in us: ";
        std::uint64_t avg (0), count (0);


        for (auto const & d : distribution) {
            std::cout << "(" << d.first << " => " << d.second << ")" << std::endl;
            avg += d.first * d.second;
            count += d.second;
        }

        //Accumulating Link Capacity for each flow
        float Link_Capacity;
        float Throughput_avg,Throughput_deviation;

        Link_Capacity = (float(pktSize * 8) / txTime.data().avg);

       	if(OLd_numPkts==numPkts){
       	}
       	else{

            auto data (ThroughputDiffs.data());
            std::cout << "Results="<< " " << OLd_numPkts << " " << (Throughput_avg=data.avg) << " " << (Throughput_deviation=data.stddev) << std::endl;
            //cout << "max=" << (max=data.max) << endl ;
            //cout << "min=" << (min=data.min) << endl ;
            //cout << "Total_avg=" << (avg=data.avg) << endl ;
            //cout << "st_deviation=" << (deviation=data.stddev) << endl ;
            //cout << "count=" << (data_count=data.cnt) << endl ;
            //cout << "Total_Packets=" << OLd_numPkts << endl;

            ThroughputDiffs.clear();
            OLd_numPkts+=10;

       	}
       	ThroughputDiffs.accumulate(Link_Capacity);

        std::cout << ", avg " << (avg / count) << std::endl;
        std::cout << "stats " << txTime.data() << ", Link_Capacity " << Link_Capacity << " Gbps" << std::endl;




    }
};


int main(int argc, char const * argv[])
{
    Configuration configuration;

    if (!configuration.parse( argc, argv)) {
        exit(1);
    }

    // determine MAC of outgoing interface
    senf::NetdeviceController netdevCtrl (configuration.interface);
    netdevCtrl.up();
    senf::MACAddress macAddr(netdevCtrl.hardwareAddress());

    if (configuration.destination) {
        // we are supposed to run as a generator
        senf::ConnectedMMapPacketSocketHandle socket (configuration.interface, 1024, 4096);
        socket.protocol().sndbuf(configuration.sendBuffer);
        std::cout << "*** Using sndBuf size of " << socket.protocol().sndbuf() << " bytes. Max burstLength is " << socket.protocol().sndbuf() / configuration.pktSize << std::endl;
        senf::ppi::module::PassiveQueueSocketSink<> sink(socket);
        Generator generator(macAddr, configuration);
        senf::ppi::connect( generator, sink);
        try {
        senf::ppi::run();
        } catch(...) {};
        auto ts (socket.protocol().txStats());
        ts.dump(std::cout); std::cout << std::endl;
    } else {
        senf::ConnectedMMapPacketSocketHandle socket (configuration.interface, 1024, 4096);
        senf::ppi::module::ActiveQueueSocketSource<senf::EthernetPacket> source (socket);
        try {
            socket.protocol().timestamping(SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE);
            netdevCtrl.timestamping(HWTSTAMP_TX_OFF, HWTSTAMP_FILTER_ALL);
            std::cerr << "Switching to RX_HARDWARE timestamping" << std::endl;
        } catch(senf::Exception & ex) {
            std::cerr << "Can not enable hw rx timestamping to due " << ex.what() << std::endl;
            std::cerr << "Switching to RX_SOFTWARE timestamping" << std::endl;
            try {
                socket.protocol().timestamping(SOF_TIMESTAMPING_RX_SOFTWARE);
            } catch(senf::Exception & ex) {
                std::cerr << "Can not enable software rx timestamping to due " << ex.what() << std::endl;
                std::cerr << "Continueing with kernel defaults" << std::endl;
            }
        }
        source.maxBurst(configuration.numPackets);
        Analyzer analyzer(macAddr, configuration);
        senf::ppi::connect( source, analyzer);
        senf::ppi::run();
    }
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
