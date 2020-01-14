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


#ifndef __PACKETSTATISTICS_HH__
#define __PACKETSTATISTICS_HH__

// Custom includes
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>
#include <senf/Utils/StatisticAccumulator.hh>
#include <senf/Utils/FlowStatistics.hh>
#include <senf/Scheduler/ClockService.hh>
#include "MGENPacket.hh"
#include "IperfPacket.hh"
#include "MPLSPacket.hh"
#include "TIMPacket.hh"

///////////////////////////////hh.p////////////////////////////////////////

class PacketStatistics
{
    typedef senf::StatisticAccumulator<std::int64_t> Accumulator;

public:
    enum Type { RECEIVED, CORRUPT, DATA, CTRL, MNGT, OTHER, IPERF, MGEN, TIM };

    Accumulator rssi;
    Accumulator rate;
    Accumulator length;
    std::uint64_t retries;
    std::uint64_t aggregated;
    senf::ClockService::clock_type airtime;

    PacketStatistics();
    
    virtual void clear();
    bool analyze(senf::AnnotationsPacket const & ap, std::uint16_t payloadSize);

    virtual void dump(std::ostream & os, senf::ClockService::clock_type const & actualDuration, bool csv);
};
   
class FlowStatistics
    : public PacketStatistics
{
public:
    senf::SequenceNumberStatistics seqNoStats;
    senf::TimestampStatistics tstampStats;
    
    FlowStatistics(std::uint8_t Sbits, std::int32_t SmaxLate, std::uint8_t Tbits);

    void clear();
    bool analyze(senf::AnnotationsPacket const & ap, std::uint16_t payloadSize, std::uint32_t seqNo, std::uint32_t txTSamp, std::uint32_t rxTStamp);

    void dump(std::ostream & os, senf::ClockService::clock_type const & actualDuration, bool csv);
};

class FlowStatisticsMGEN
    : public FlowStatistics
{
public:
    FlowStatisticsMGEN();

    bool analyze(MGENPacket const & mgen, senf::AnnotationsPacket const & ap);
};

class FlowStatisticsIPERF
    : public FlowStatistics
{
public:
    FlowStatisticsIPERF();

    bool analyze(IperfUDPPacket const & iperf, senf::AnnotationsPacket const & ap);
};

class FlowStatisticsTIM
    : public FlowStatistics
{
public:
    FlowStatisticsTIM();

    bool analyze(senf::TIMPacket const & tim, senf::AnnotationsPacket const & ap);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif
