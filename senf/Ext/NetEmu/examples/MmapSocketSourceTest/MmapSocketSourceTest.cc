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
//     Mathias Kretschmer <mtk@berlios.de>


// Definition of non-inline non-template functions

// Custom includes
#include <fstream>
#include <string>
#include <iomanip>
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/PPI/PPI.hh>
#include <senf/PPI/Connectors.hh>
#include <senf/PPI/SocketSink.hh>
#include <senf/PPI/DiscardSink.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>
#include <boost/unordered/unordered_map.hpp>

#include <senf/Socket/Protocols/Raw/PacketSocketHandle.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>
#include <senf/Ext/NetEmu/WLAN/MmapSocketSourceRadioTap.hh>
#include <senf/Ext/NetEmu/Ethernet/MmapSocketSourceEthernet.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>
#include <senf/Ext/NetEmu/WLAN/MonitorDataFilter.hh>
#include <senf/Socket/NetdeviceController.hh>

#include "Configuration.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

class Analyzer : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Analyzer);

    void request() {
        senf::EthernetPacket eth (input());

        PayloadMap::iterator it(payloadMap_.find(eth->type_length()));
        if (it == payloadMap_.end()) {
            payloadMap_.insert( std::make_pair(eth->type_length(), 1));
        } else {
            it->second++;
        }

        if (logSeqNoErrors_) {
            senf::emu::annotations::Quality const & qual (eth.annotation<senf::emu::annotations::Quality>());
            if (qual.flags.framePredecessorLost > 0) {
                std::cout << "(" << unsigned(qual.flags.framePredecessorLost) << ")" << std::flush;
            }
            else {
                std::cout << "." << std::flush;
            }
        }

        if (eth->type_length() == 0x8847) {
            senf::MACAddress tmp(eth->source());
            eth->source() << eth->destination();
            eth->destination() << tmp;
            output(eth);
        }
    }

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
    senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

    bool logSeqNoErrors_;
    typedef boost::unordered_map<boost::uint16_t, unsigned> EtherTypeMap;
    EtherTypeMap payloadMap_;

    Analyzer(bool logSeqNoErrors)  : 
        logSeqNoErrors_(logSeqNoErrors)
    {
        route(input,output).autoThrottling( false);
        input.onRequest( &Analyzer::request);
        input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
    }
    
    void dump(std::ostream &os) {
        os << "(";
        for (EtherTypeMap::value_type & entry : payloadMap_) {
            os << std::hex << "0x" << entry.first << "/" << std::dec << entry.second << ";";
        }
        os << ")";
        payloadMap_.clear();
    }
};

class Forwarder : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Forwarder);

    void request() {
        senf::EthernetPacket eth (input());
        if (eth->type_length() != 0) {
            output(eth);
        }
    }

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
    senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

    Forwarder()
    {
        route(input,output).autoThrottling( false);
        input.onRequest( &Forwarder::request);
        input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
    }
};


template<class SOURCE_TYPE>
class Dummy
{
    senf::PacketSocketHandle socket;
    SOURCE_TYPE source;
    Analyzer analyzer;
    Forwarder forwarder1, forwarder2;
    senf::emu::MonitorDataFilter monitorDataFilter;
    senf::scheduler::TimerEvent timer;
    senf::ppi::module::DiscardSink discardSink;
    senf::ppi::module::DiscardSink discardSink2;
    senf::PacketSocketHandle socketOut;
    senf::ppi::module::PassiveSocketSink<senf::ppi::TargetLLSocketWriter> sink;

public:
    Dummy( Configuration & configuration) : 
    source( socket, configuration.numBuffers),
    analyzer( configuration.logSeqNoErrors),
    timer( "report", boost::bind(&Dummy::report, this)),
    sink(socketOut)
        {
            socket.bind(senf::LLSocketAddress(configuration.interface));
            
            senf::ppi::connect( source, monitorDataFilter);
            senf::ppi::connect( monitorDataFilter, forwarder1);
            senf::ppi::connect( monitorDataFilter.monitor, discardSink2); // discard ctrl/mgnt frames
            senf::ppi::connect( forwarder1, analyzer);
            senf::ppi::connect( analyzer,forwarder2);

            if (configuration.outputInterface != "") {
                socketOut.bind(senf::LLSocketAddress(configuration.outputInterface));
                senf::ppi::connect( forwarder2, sink);
            }
            else {
                senf::ppi::connect( forwarder2, discardSink);
            }

            senf::NetdeviceController netdevCtrl (configuration.interface);
            monitorDataFilter.id(netdevCtrl.hardwareAddress());
            monitorDataFilter.promisc(true);

            report();
        }
    
    void report() {
        SENF_LOG_BLOCK((senf::log::MESSAGE)({
                    log << "rxDropped " << source.dropped();
                    log << ", filterStats ";  monitorDataFilter.stats().dump(log);
                    log << ", etherTypeMap ";  analyzer.dump(log);
                }));
        
        timer.timeout( senf::scheduler::now() + senf::ClockService::seconds(1));
    }
};

int main(int argc, char const * argv[])
{
    Configuration configuration;

    if (!configuration.parse( argc, argv)) {
        exit(1);
    }

    if (configuration.monitorInterface != "") {
        configuration.interface = configuration.monitorInterface;
        Dummy<senf::MmapSocketSourceRadioTap> dummy( configuration);
        senf::ppi::run();
    }
    else {
        Dummy<senf::MmapSocketSourceEthernet> dummy( configuration);
        senf::ppi::run();
    }
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MmapSocketSourceTespp.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
