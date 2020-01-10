//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include "FrameAnalyzer.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>
#include "80211Analyzer.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ FrameAnalyzer::FrameAnalyzer(Configuration const & configuration)
    : configuration_(configuration),
      timer_("reportingTimer", senf::membind( &FrameAnalyzer::timerEvent, this)),
      initWait_("maxWaitTimer", senf::membind( &FrameAnalyzer::initWaitEvent, this)),
      startTime_(senf::ClockService::milliseconds(0)),
      nextTimeout_(senf::ClockService::milliseconds(0)),
      numPackets_(0),
      mplsDetected_(false)
{
    noroute(input);
    input.onRequest( &FrameAnalyzer::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
    if (configuration.maxWait != senf::ClockService::milliseconds(0))
        initWait_.timeout( senf::scheduler::now() + configuration.maxWait);
}

prefix_ void FrameAnalyzer::timerEvent()
{
    // get current time
    senf::ClockService::clock_type current (senf::ClockService::now());
    // call report with current time and the actual interval duration
    if (startTime_ > senf::ClockService::clock_type(0)) {
        report(current - startTime_, configuration_.reportingInterval + (current - nextTimeout_));
    }
    // restart timer fixing any potential scheduling issues
    nextTimeout_ += configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);
}

prefix_ void FrameAnalyzer::initWaitEvent()
{
    SENF_LOG((senf::log::IMPORTANT)("No packet received, terminating..."));
    exitCode(1);
    senf::scheduler::terminate();
}

prefix_ void FrameAnalyzer::resetStartTime()
{
    SENF_LOG((senf::log::IMPORTANT)("Here we go..."));

    startTime_ = senf::ClockService::now();
    nextTimeout_ = startTime_ + configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);
    initWait_.disable();
}

prefix_ void FrameAnalyzer::request()
{
    if (startTime_ > senf::ClockService::clock_type(0) && configuration_.duration > senf::ClockService::clock_type(0)) {
        // we've been started
        if ((senf::scheduler::now() - startTime_) >= configuration_.duration) {
            senf::scheduler::terminate();
        }
    }

    // Look for the Annotation Packet. If we have one, we have receiving cooked 802.11 frames...
    senf::EthernetPacket const & ethIn (input());
    senf::AnnotationsPacket const & ap (ethIn.find<senf::AnnotationsPacket>(senf::nothrow));
    if (!ap) {
        // If not, we receive plain data frames (Ethernet)
        flowStats(PacketStatistics::RECEIVED, 0)->analyze(ap, ethIn.size());
        // let see, if we understand them
        if (!handleDataFrame(ethIn, ap)) {
            flowStats(PacketStatistics::OTHER, 0)->analyze(ap, ethIn.size());
        }
        return;
    }

    flowStats(PacketStatistics::RECEIVED, 0)->analyze(ap, ap.next().size());
    
    if (ap->corrupt()) {
        flowStats(PacketStatistics::CORRUPT, 0)->analyze(ap, ap.next().size());
        return;
    }

    if (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate == 0) {
        flowStats(PacketStatistics::OTHER, 0)->analyze(ap, ap.next().size());
        return;
    }
    
    senf::WLANPacket_MgtFrame const & wlanm (ap.next<senf::WLANPacket_MgtFrame>(senf::nothrow));
    if (wlanm) {
        flowStats(PacketStatistics::MNGT, 0)->analyze(ap, wlanm.size());
        return;
    }

    senf::WLANPacket_CtrlFrame const & wlanc (ap.next<senf::WLANPacket_CtrlFrame>(senf::nothrow));
    if (wlanc) {
        flowStats(PacketStatistics::CTRL, 0)->analyze(ap, wlanc.size());
        return;
    }

    // nonQoSData WLAN frames
    senf::WLANPacket_DataFrame const & data (ap.next<senf::WLANPacket_DataFrame>(senf::nothrow));
    if (data) {
        flowStats(PacketStatistics::OTHER, 0)->analyze(ap, data.size());
        return;
    }

    senf::EthernetPacket const & eth (ap.next<senf::EthernetPacket>(senf::nothrow));
    if (eth) {
        if (handleDataFrame(eth, ap))
            return;
    }

    flowStats(PacketStatistics::OTHER, 0)->analyze(ap, ap.next().size());
}

//////////////////////////////////////////////////

prefix_ bool FrameAnalyzer::handleDataFrame(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap)
{
    flowStats(PacketStatistics::DATA, 0)->analyze(ap, eth.size());

    senf::MPLSPacket const & mpls (eth.next<senf::MPLSPacket>(senf::nothrow));
    if (mpls) {
        senf::TIMPacket const & tim (mpls.next<senf::TIMPacket>(senf::nothrow));
        if (tim) {
            if (handleMPLSPacket(eth, ap, mpls, tim)) {
                if (configuration_.numPackets > 0) {
                    if (++numPackets_ >= configuration_.numPackets) {
                        senf::scheduler::terminate();
                    }
                }
                return true;
            }
        }
    }
    
    senf::IPv4Packet const & ip4 (eth.next<senf::IPv4Packet>(senf::nothrow));
    if (ip4) {
        senf::UDPPacket const & udp (ip4.next<senf::UDPPacket>(senf::nothrow));
        if (udp) {
            if (handleUDPPacket( eth, ap, ip4, udp)) {
                if (configuration_.numPackets > 0) {
                    if (++numPackets_ >= configuration_.numPackets) {
                        senf::scheduler::terminate();
                    }
                }
                return true;
            }
        }
    }

    return false;
}

prefix_ bool FrameAnalyzer::handleMPLSPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
        senf::MPLSPacket const& mpls, senf::TIMPacket const & tim)
{
    mplsDetected_ = true;
    return ((FlowStatisticsTIM*)flowStats(FlowStatistics::TIM, mpls->label()))->analyze(tim, ap);
}

prefix_ bool FrameAnalyzer::handleUDPPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap,
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
            return ((FlowStatisticsMGEN*)flowStats(FlowStatistics::MGEN, mgen->flowId()))->analyze(mgen, ap);
        }
    }
    else if (configuration_.iperfMode) {
        IperfUDPPacket const & iperf (udp.parseNextAs<IperfUDPPacket>());
        if (iperf) {
            return ((FlowStatisticsIPERF*)flowStats(FlowStatistics::IPERF, 0))->analyze(iperf, ap);
        }
    }

    return false;
}

prefix_ PacketStatistics * FrameAnalyzer::flowStats(PacketStatistics::Type type, std::uint32_t flowId)
{
    auto key (std::make_pair(type, flowId));

    auto it (packetStatsMap.find(key));
    if (it != packetStatsMap.end()) {
        return it->second;
    }

    // first flow created (first frame seen)
    if (packetStatsMap.empty()) {
        resetStartTime();
    }

    switch (type) {
    case PacketStatistics::RECEIVED:
    case PacketStatistics::CORRUPT:
    case PacketStatistics::DATA:
    case PacketStatistics::CTRL:
    case PacketStatistics::MNGT:
    case PacketStatistics::OTHER:
        packetStatsMap.insert(key, new PacketStatistics());
        break;
    case PacketStatistics::IPERF:
        packetStatsMap.insert(key, new FlowStatisticsIPERF());
        break;
    case PacketStatistics::MGEN:
        packetStatsMap.insert(key, new FlowStatisticsMGEN());
        break;
    case PacketStatistics::TIM:
        packetStatsMap.insert(key, new FlowStatisticsTIM());
        break;
    }

    return packetStatsMap.find(key)->second;
}


prefix_ void FrameAnalyzer::resetStats()
{
    for (auto const & ps : packetStatsMap) {
        ps.second->clear();
    }
}

static std::string flowIdAsString(PacketStatistics::Type type, std::uint32_t flowId)
{
    std::string label ("?!?UNKNOWN?!?");
    switch (type) {
    case PacketStatistics::RECEIVED:
        label = "RECEIVED " + senf::str(flowId);
        break;
    case PacketStatistics::CORRUPT:
        label = "CORRUPT " + senf::str(flowId);
        break;
    case PacketStatistics::DATA:
        label = "DATA " + senf::str(flowId);
        break;
    case PacketStatistics::CTRL:
        label = "CTRL " + senf::str(flowId);
        break;
    case PacketStatistics::MNGT:
        label = "MNGT " + senf::str(flowId);
        break;
    case PacketStatistics::OTHER:
        label = "OTHER " + senf::str(flowId);
        break;
    case PacketStatistics::IPERF:
        label = "IPERF " + senf::str(flowId);
        break;
    case PacketStatistics::MGEN:
        label = "MGEN " + senf::str(flowId);
        break;
    case PacketStatistics::TIM: 
        label = "TIM " + senf::str(flowId);
        break;
    }

    label.resize(16, ' ');
    
    return label;
}

prefix_ void FrameAnalyzer::report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration)
{
    for (auto const & ps : packetStatsMap) {
        if (configuration_.csvMode) {
            std::cout << flowIdAsString(ps.first.first, ps.first.second) << "," << senf::ClockService::in_milliseconds(timestamp);
            ps.second->dump(std::cout, actualDuration, true);
        } else {
            std::cout << flowIdAsString(ps.first.first, ps.first.second) << " " << senf::ClockService::in_milliseconds(timestamp);
            ps.second->dump(std::cout, actualDuration, false);
        }
        std::cout << std::endl;
    }

    resetStats();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
