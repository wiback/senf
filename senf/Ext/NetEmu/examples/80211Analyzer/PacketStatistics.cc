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
    
    retries += ap->retransmitted();
    aggregated += ap->aggregated();
    
    if (ap->modulationId() != 0) {
        unsigned rateInBps (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate);
        rate.accumulate(rateInBps);
        airtime += senf::ClockService::microseconds( (ap->length() * 8 * 1000) / rateInBps);  // need to add preamble
    }

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
