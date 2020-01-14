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

