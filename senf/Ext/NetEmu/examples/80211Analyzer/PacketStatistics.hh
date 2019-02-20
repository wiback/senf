// $Id:$
//
// Copyright (C) 2012
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

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
