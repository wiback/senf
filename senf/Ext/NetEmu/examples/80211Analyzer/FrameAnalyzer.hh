// $Id:$
//
// Copyright (C) 2012
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

#ifndef __FRAMEANALYZER_HH__
#define __FRAMEANALYZER_HH__

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>
#include <senf/PPI/Module.hh>
#include "PacketStatistics.hh"
#include "Configuration.hh"

///////////////////////////////hh.p////////////////////////////////////////

class FrameAnalyzer
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(FrameAnalyzer);

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;

    FrameAnalyzer(Configuration const & configuration);

protected:
    Configuration const & configuration_;
    boost::ptr_map<std::pair<PacketStatistics::Type,std::uint32_t>,PacketStatistics> packetStatsMap;

    PacketStatistics *flowStats(PacketStatistics::Type, std::uint32_t flowId);
    
    void resetStats();

    void resetStartTime();
    senf::ClockService::clock_type const & startTime() {
        return startTime_;
    }

private:
    senf::scheduler::TimerEvent    timer_;
    senf::scheduler::TimerEvent    initWait_;
    senf::ClockService::clock_type startTime_;
    senf::ClockService::clock_type nextTimeout_;
    std::uint64_t                  numPackets_;
    bool                           mplsDetected_;

    void request();
    void timerEvent();
    void initWaitEvent();

    bool handleDataFrame(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap);

    bool handleUDPPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
                         senf::IPv4Packet const & ip4, senf::UDPPacket const & udp);
    bool handleMPLSPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
                          senf::MPLSPacket const & mpls, senf::TIMPacket const & tim);

    void report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

