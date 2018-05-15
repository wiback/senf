// $Id:$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DataPacket.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>

#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/PPI/QueueSocketSourceSink.hh>
#include <senf/PPI/PPI.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Socket/NetdeviceController.hh>

#include "ConfigurationA.hh"
#include "InternalThroughputTestPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


class Analyzer : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Analyzer);

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
    senf::MACAddress ourMAC_;
    std::vector<std::int64_t> tstamps;
    unsigned sessionId_;

    Analyzer(senf::MACAddress const & mac, Configuration const & config) :
        ourMAC_(mac),
        sessionId_(0)
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
                    SENF_LOG( (senf::log::IMPORTANT) ("Packet #" << testPkt->seqNo() << " ignored due to mismatching sessionId") );
                    return;
                }
 
                // we have a valid packet
                auto rxTstamp (senf::ClockService::in_nanoseconds(eth.annotation<senf::emu::annotations::Timestamp>().as_clock_type()));
                
                SENF_LOG( (senf::log::IMPORTANT) ("Packet #" << testPkt->seqNo() << "/" << testPkt->numPkts() << " received with length " << eth.size()
                                                  << ", timestamp " << rxTstamp) );
                
                if (testPkt->seqNo() < testPkt->numPkts()) { 
                    tstamps.push_back(rxTstamp);
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
        std::cerr << "tstamps ";
        for (auto const & ts : tstamps) {
            std::cerr << ts << " ";
        }
        std::cerr << std::endl;
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

    // qlen 512, frameSize 4096
    senf::ConnectedMMapPacketSocketHandle socket (configuration.interface, 1024, 4096);
    senf::ppi::module::ActiveQueueSocketSource<senf::EthernetPacket> source (socket);

    Analyzer analyzer(macAddr, configuration);

    senf::ppi::connect( source, analyzer);
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
