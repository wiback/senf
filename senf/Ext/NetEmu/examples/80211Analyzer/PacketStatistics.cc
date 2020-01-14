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


#include "PacketStatistics.hh"

// Custom includes
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


prefix_ PacketStatistics::PacketStatistics()
{
    clear();
}

prefix_ void PacketStatistics::clear()
{
    length.clear();
    rate.clear();
    rssi.clear();

    airtime = senf::ClockService::clock_type(0);
    retries = aggregated = 0;
}

prefix_ bool PacketStatistics::analyze(senf::AnnotationsPacket const & ap, std::uint16_t payloadSize)
{
    length.accumulate(payloadSize);

    if (!ap)
        return true;
    
    retries    += ap->retransmitted();
    aggregated += ap->aggregated();
    airtime    += senf::ClockService::microseconds(ap->airTime());

    if (ap->rssi() != 0) {
        rssi.accumulate(ap->rssi());
    }

    return true;
}

static std::string formatEng(senf::StatisticsData const & data)
{
    return senf::str(senf::format::eng(data.avg, data.stddev).setprecision(6).showpoint().uppercase());
}

static std::string formatCSV(senf::StatisticsData const & data)
{
    std::stringstream ss;
    if (data.cnt == 0)
        ss << NAN << "," << NAN << "," << NAN << "," << NAN;
    else
        ss << data.min << "," << data.avg << "," << data.max << "," << data.stddev;

    return ss.str();
}


prefix_ void PacketStatistics::dump(std::ostream & os, senf::ClockService::clock_type const & actualDuration, bool csv)
{
    if (csv) {
    } else {
        os << " pkts " << length.count() << " size " << formatEng(length.data()) << " rssi " << formatEng(rssi.data()) << " rate " << formatEng(rate.data())
           << " airTime " << senf::ClockService::in_microseconds(airtime) << "us retries " << retries << " aggregated " << aggregated;
    }
}

prefix_ FlowStatistics::FlowStatistics(std::uint8_t Sbits, std::int32_t SmaxLate, std::uint8_t Tbits)
    : seqNoStats(Sbits, SmaxLate),
      tstampStats(Tbits)
{
    clear();
}

prefix_ void FlowStatistics::clear()
{
    PacketStatistics::clear();

    seqNoStats.clear();
    tstampStats.clear();
}

prefix_ bool FlowStatistics::analyze(senf::AnnotationsPacket const & ap, std::uint16_t payloadSize, std::uint32_t seqNo, std::uint32_t txTSamp, std::uint32_t rxTStamp)
{
    if (!PacketStatistics::analyze(ap, payloadSize))
        return false;
    
    seqNoStats.update(seqNo, payloadSize);
    tstampStats.update(txTSamp, rxTStamp, true);

    return true;
}

prefix_ void FlowStatistics::dump(std::ostream & os, senf::ClockService::clock_type const & actualDuration, bool csv)
{
    PacketStatistics::dump(os, actualDuration, csv);
    
    if (csv) {
    } else {
        os << " good " << seqNoStats.good << " goodBytes " << seqNoStats.goodBytes << " late " << seqNoStats.late << "/" << seqNoStats.lateMax << " duplicate " << seqNoStats.duplicate
           << " lost " << seqNoStats.lost << " lostLate " << seqNoStats.lostLate << " resyncs " << seqNoStats.resyncs << " pdv " << formatEng(tstampStats.pdv.data());
    }
}


prefix_ FlowStatisticsMGEN::FlowStatisticsMGEN()
    : FlowStatistics(32, 128, 32)
{
}

prefix_ bool FlowStatisticsMGEN::analyze(MGENPacket const & mgen, senf::AnnotationsPacket const & ap)
{
    std::uint32_t txTSamp (std::int64_t(mgen->txTimeSeconds()) * 1000 + std::int64_t(mgen->txTimeMicroseconds()) / 1000);
    std::uint32_t rxTSamp (mgen.annotation<senf::emu::annotations::Timestamp>().as_milli_seconds());
    return FlowStatistics::analyze(ap, mgen.size(), mgen->sequenceNumber(), txTSamp, rxTSamp);
}

prefix_ FlowStatisticsIPERF::FlowStatisticsIPERF()
    : FlowStatistics(32, 128, 32)
{
}

prefix_ bool FlowStatisticsIPERF::analyze(IperfUDPPacket const & iperf, senf::AnnotationsPacket const & ap)
{
    std::uint32_t txTSamp (std::int64_t(iperf->tv_sec()) * 1000 + std::int64_t(iperf->tv_usec()) / 1000);
    std::uint32_t rxTSamp (iperf.annotation<senf::emu::annotations::Timestamp>().as_milli_seconds());
    return FlowStatistics::analyze(ap, iperf.size(), iperf->id(), txTSamp, rxTSamp);
}

prefix_ FlowStatisticsTIM::FlowStatisticsTIM()
    : FlowStatistics(22, 128, 14)
{
}

prefix_ bool FlowStatisticsTIM::analyze(senf::TIMPacket const & tim, senf::AnnotationsPacket const & ap)
{
    std::uint32_t txTSamp (tim->timestamp());
    std::uint32_t rxTSamp (tim.annotation<senf::emu::annotations::Timestamp>().as_milli_seconds(senf::TIMPacketParser::timestamp_t::max_value + 1));
    return FlowStatistics::analyze(ap, tim.size(), tim->sequenceNumber(), txTSamp, rxTSamp);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
