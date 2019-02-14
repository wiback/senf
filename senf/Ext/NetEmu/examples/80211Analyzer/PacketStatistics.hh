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
#include <senf/Scheduler/ClockService.hh>
#include "MGENPacket.hh"
#include "IperfPacket.hh"
#include "MPLSPacket.hh"
#include "TIMPacket.hh"

///////////////////////////////hh.p////////////////////////////////////////

class PacketStatistics
{
public:
    boost::uint64_t count;
    boost::uint64_t bytes;
    boost::uint64_t retry;

    senf::ClockService::clock_type airtime;

    PacketStatistics();
    virtual ~PacketStatistics() {}

    virtual void v_reset();
    virtual bool v_analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize = 0);
    bool analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize);

    float pktsPerSecond(senf::ClockService::clock_type reportingInterval);
    float bitsPerSecond(senf::ClockService::clock_type reportingInterval);
    float retryPerSecond(senf::ClockService::clock_type reportingInterval);
    senf::ClockService::clock_type airtimePerSecond(senf::ClockService::clock_type reportingInterval);
};


template <class T>
class LossCalculator
{
private:
    T maxSeq_;
    T lastSeq_;
    T threshold_;
    bool initialized_;

    T pktsGood_;
    T pktsLost_;
    T pktsLate_;
    T pktsDuplicate_;

    int seqDiff(T last, T current) {
        int dist = current - last;
        if (dist + int(threshold_) < 0)
            return dist + maxSeq_;
        if (dist - int(maxSeq_ + threshold_) > 0)
            return dist - maxSeq_;
        return dist;
    }

public:
    void update(T newSeq) {
        if (!initialized_) {
            initialized_ = true;
            lastSeq_ = newSeq;
            pktsGood_ = 1;
            return;
        }

        int diff = seqDiff( lastSeq_, newSeq);

        if (diff == 1) {
            // no loss
            pktsGood_++;
            lastSeq_ = newSeq;
        }
        else if (diff == 0) {
            // duplicate
            pktsDuplicate_++;
        }
        else if (diff < 0) {
            // late frame(s) => reordering
            pktsLate_++;
        } else {
            // frame loss
            pktsLost_+= diff-1;
            lastSeq_ = newSeq;
        }
    }

    LossCalculator(T _maxSeq)
        :  maxSeq_(_maxSeq)
    {
        initialized_ = false;
        threshold_ = maxSeq_ / 10;

        reset();
    }

    void reset() {
        pktsGood_ = 0;
        pktsLost_ = 0;
        pktsLate_ = 0;
        pktsDuplicate_ = 0;
    }

    float getLoss() {
        if (pktsGood_ == 0)
            return 0.0f;
        else
            return float(pktsLost_) / float(pktsGood_ + pktsLost_);
    }

    T getPktsDuplicate() {
        return pktsDuplicate_;
    }

    T getPktsLate() {
        return pktsLate_;
    }
};


typedef senf::StatisticAccumulator<boost::int64_t> Accumulator;

class FlowStatistics : public PacketStatistics
{
public:
    Accumulator rssi;
    Accumulator noise;
    Accumulator rate;
    Accumulator length;
    Accumulator latency;
    LossCalculator<boost::uint32_t> loss;

    FlowStatistics();

    void v_reset();
    bool v_analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize = 0);
    bool v_analyze(MGENPacket const & mgen, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime);
    bool v_analyze(IperfUDPPacket const & iperf, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime);
    bool v_analyze(senf::TIMPacket const & tim, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime);

    void getRssi(senf::StatisticsData & data);
    void getNoise(senf::StatisticsData & data);
    void getRate(senf::StatisticsData & data);
    void getLength(senf::StatisticsData & data);
    void getLatency(senf::StatisticsData & data);
    float getLoss();
    boost::int64_t getPktsDuplicate();
    boost::int64_t getPktsLate();
};


///////////////////////////////hh.p////////////////////////////////////////
#endif
