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

#include "MGENAnalyzer.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ MGENAnalyzer::MGENAnalyzer(Configuration const & configuration) :
    FrameAnalyzer( configuration)
{
}

prefix_ FlowStatistics & MGENAnalyzer::getFlowStats(boost::uint32_t flowId)
{
    FlowStatisticsMap::iterator it (flowStatsMap.find( flowId));
    if (it != flowStatsMap.end()) {
        return it->second;
    }

    // first flow created (first frame seen)
    if (flowStatsMap.size() == 0) {
        resetStartTime();
    }

    flowStatsMap.insert( std::make_pair( flowId, FlowStatistics()));
    return flowStatsMap.find( flowId)->second;
}

prefix_ bool MGENAnalyzer::v_handleMPLSPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
        senf::MPLSPacket const& mpls, senf::TIMPacket const & tim)
{
    getFlowStats(mpls->label()).v_analyze(tim, ap, tim.size(), configuration_.clockDrift, startTime());
}

prefix_ bool MGENAnalyzer::v_handleUDPPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
        senf::IPv4Packet const& ip4, senf::UDPPacket const & udp)
{
    // check for matching source addr/port
    if (((configuration_.source.address() != senf::INet4Address::None) && (configuration_.source.address() != ip4->destination().value())) ||
            ((configuration_.source.port() != 0) && (configuration_.source.port() != udp->destination()))) {
        return false;
    }

    // check for matching destination addr/port
    if (((configuration_.destination.address() != senf::INet4Address::None) && (configuration_.destination.address() != ip4->destination().value())) ||
            ((configuration_.destination.port() != 0) && (configuration_.destination.port() != udp->destination()))) {
        return false;
    }

    if (configuration_.mgenMode) {
        MGENPacket const & mgen (udp.parseNextAs<MGENPacket>());
        if (mgen) {
            getFlowStats(mgen->flowId()).v_analyze(mgen, ap, udp.size(), configuration_.clockDrift, startTime());
        } else {
            SENF_LOG((senf::log::CRITICAL)("Failed to parse mgen header"));
            exit(1);
        }
    } else {
        if (configuration_.iperfMode) {
            IperfUDPPacket const & iperf (udp.parseNextAs<IperfUDPPacket>());
            if (iperf) {
                getFlowStats(0).v_analyze(iperf, ap, udp.size(), configuration_.clockDrift, startTime());
            } else {
                SENF_LOG((senf::log::CRITICAL)("Failed to parse iperf header"));
                exit(1);
            }
        } else {
            getFlowStats(0).v_analyze( udp, ap, udp.size());
        }
    }

    return true;
}

prefix_ void MGENAnalyzer::v_report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration)
{
    FlowStatisticsMap::iterator it;
    for (it = flowStatsMap.begin(); it != flowStatsMap.end(); it++) {
        if (configuration_.csvMode) {
            std::cout << it->first << ",";
            reportCSV( it->second, timestamp, actualDuration);
        } else {
            std::cout << "flowId " << it->first << " ";
            reportHuman( it->second, timestamp, actualDuration);
        }
    }

    // now, reset the flow stats
    for (it = flowStatsMap.begin(); it != flowStatsMap.end(); it++) {
        it->second.v_reset();
    }

    resetStats();
}

prefix_ float MGENAnalyzer::sumMgenFlow(senf::ClockService::clock_type const & actualDuration)
{
    float sum = 0.0;

    FlowStatisticsMap::iterator it;
    for (it = flowStatsMap.begin(); it != flowStatsMap.end(); it++) {
        sum += it->second.pktsPerSecond( actualDuration);
    }

    return sum;
}


prefix_ void MGENAnalyzer::reportHuman(FlowStatistics & flow, senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration)
{
    std::cout << senf::ClockService::in_milliseconds( timestamp) << " "
              << "pps " << flow.pktsPerSecond( actualDuration) << " "
              << "kbps "  << flow.bitsPerSecond( actualDuration) / 1000 << " "
              << "retry "  << flow.retryPerSecond( actualDuration) << " ";
    senf::StatisticsData data_;
    flow.getLength( data_);
    std::cout << "length "  << senf::str( senf::format::eng( data_.avg, data_.stddev).setprecision(6).showpoint().uppercase()) << " ";
    flow.getRssi( data_);
    std::cout << "rssi "  << senf::str( senf::format::eng( data_.avg, data_.stddev).setprecision(4).showpoint().uppercase()) << " ";
    flow.getRate( data_);
    std::cout << "mcs "  << senf::str( senf::format::eng( data_.avg, data_.stddev).setprecision(5).showpoint().uppercase()) << " ";

    if (configuration_.mgenMode) {
        flow.getLatency( data_);
        std::cout << "latency "  << senf::str( senf::format::eng( data_.avg, data_.stddev).setprecision(5).showpoint().uppercase()) << " "
                  << "loss "  << flow.getLoss() * 100.0 << "% "
                  << "duplicate "  << flow.getPktsDuplicate() << " "
                  << "late "  << flow.getPktsLate() << " ";
    }

    if (configuration_.verbose) {
        std::cout << "received "    << received.pktsPerSecond( actualDuration) << " "
                  << "data "        << data.pktsPerSecond( actualDuration) << " "
                  << "corrupt "     << corrupt.pktsPerSecond( actualDuration) << " "
                  << "control "     << control.pktsPerSecond( actualDuration) << " "
                  << "management "  << management.pktsPerSecond( actualDuration) << " "
                  << "other "       << other.pktsPerSecond( actualDuration) << " "
                  << "dataNonUDP "  << dataNonUDP.pktsPerSecond( actualDuration) << " "
                  << "nonMGEN "     << received.pktsPerSecond( actualDuration) - sumMgenFlow( actualDuration);
    }

    std::cout << std::endl;
}

static std::ostream & dumpCSV(std::ostream & os, senf::StatisticsData const & data)
{
    if (data.cnt == 0)
        return os << NAN << "," << NAN << "," << NAN << "," << NAN;
    else
        return os << data.min << "," << data.avg << "," << data.max << "," << data.stddev;
}


prefix_ void MGENAnalyzer::reportCSV(FlowStatistics & flow,
        senf::ClockService::clock_type const & timestamp,  senf::ClockService::clock_type const & actualDuration)
{
    std::cout << senf::ClockService::in_milliseconds( timestamp) << ","
              << flow.pktsPerSecond( actualDuration) << ","
              << flow.bitsPerSecond( actualDuration) / 1000 << ","
              << flow.retryPerSecond( actualDuration) << ",";

    senf::StatisticsData data_;
    flow.getLength( data_);
    dumpCSV( std::cout, data_) << ",";
    flow.getRssi( data_);
    dumpCSV( std::cout, data_) << ",";
    flow.getRate( data_);
    dumpCSV( std::cout, data_) << ",";

    if (configuration_.mgenMode) {
        flow.getLatency( data_);
        dumpCSV( std::cout, data_) << ",";
        std::cout << flow.getLoss() << ","
                  << flow.getPktsDuplicate() << ","
                  << flow.getPktsLate() << ",";
    }

    if (configuration_.verbose) {
        std::cout << received.pktsPerSecond( actualDuration) << ","
                  << data.pktsPerSecond( actualDuration) << ","
                  << corrupt.pktsPerSecond( actualDuration) << ","
                  << control.pktsPerSecond( actualDuration) << ","
                  << management.pktsPerSecond( actualDuration) << ","
                  << other.pktsPerSecond( actualDuration) << ","
                  << dataNonUDP.pktsPerSecond( actualDuration) << ","
                  << received.pktsPerSecond( actualDuration) - sumMgenFlow( actualDuration);
    }

    std::cout << std::endl;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
