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
#include <senf/Utils/StatisticAccumulator.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>
#include <senf/PPI/Module.hh>
#include "Configuration.hh"

///////////////////////////////hh.p////////////////////////////////////////

struct FrameStats
{
    unsigned pktCnt;
    unsigned byteCnt;
    unsigned airTime;
    senf::StatisticAccumulator<float> rssi;
    senf::StatisticAccumulator<float> rate;

    FrameStats () {
        clear();
    };

    void clear() {
        pktCnt = byteCnt = airTime = 0;
        rssi.clear();
        rate.clear();
    }

    void update (senf::AnnotationsPacket const & ap) {
        pktCnt++;
        byteCnt += ap->length();
        airTime += ap->airTime();
        if (ap->rssi() != 0)
            rssi.accumulate(ap->rssi());
        rate.accumulate(senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate);
    }

    void dump(std::ostream & os, senf::ClockService::clock_type const & period) {
        os << "pktCnt " << pktCnt << ", byteCnt " << byteCnt << ", airTime " << airTime
           << ", airTimeFraction " << float(airTime) / float(senf::ClockService::in_microseconds(period))
           << ", rssi " << rssi.data() << ", rate" << rate.data();
    }
};

class FrameAnalyzer
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(FrameAnalyzer);

public:
    senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;

    FrameAnalyzer(Configuration const & configuration);

protected:
    Configuration const & configuration_;
    
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

    FrameStats data;
    FrameStats ctrl;
    FrameStats mngt;
    FrameStats unknown;
    FrameStats norate;
    FrameStats corrupt;
    FrameStats all;
    
    void request();
    void timerEvent();
    void initWaitEvent();

    void report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

