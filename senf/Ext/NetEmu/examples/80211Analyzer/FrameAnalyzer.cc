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

#include "FrameAnalyzer.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>
#include "80211Analyzer.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ FrameAnalyzer::FrameAnalyzer(Configuration const & configuration)
    : configuration_(configuration),
      timer_("reportingTimer", senf::membind( &FrameAnalyzer::timerEvent, this)),
      initWait_("maxWait timer", senf::membind( &FrameAnalyzer::initWaitEvent, this)),
      startTime_(senf::ClockService::milliseconds(0)),
      nextTimeout_(senf::ClockService::milliseconds(0)),
      numPackets_(0)
{
    noroute(input);
    input.onRequest( &FrameAnalyzer::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
    noroute(monitor);
    monitor.onRequest( &FrameAnalyzer::requestMonitor);
    monitor.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
    if (configuration.maxWait != senf::ClockService::milliseconds(0))
        initWait_.timeout( senf::scheduler::now() + configuration.maxWait);
}

prefix_ void FrameAnalyzer::timerEvent()
{
    // get current time
    senf::ClockService::clock_type current (senf::ClockService::now());
    // call report with current time and the actual interval duration
    if (startTime_ > senf::ClockService::clock_type(0)) {
        v_report( current - startTime_, configuration_.reportingInterval + (current - nextTimeout_));
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

prefix_ void FrameAnalyzer::requestMonitor()
{
    if (startTime_ > senf::ClockService::clock_type(0) && configuration_.duration > senf::ClockService::clock_type(0)) {
        // we've been started
        if ((senf::scheduler::now() - startTime_) >= configuration_.duration) {
            senf::scheduler::terminate();
        }
    }

    senf::RadiotapPacket rt (monitor());
    senf::emu::annotations::Quality const & q (rt.annotation<senf::emu::annotations::Quality>());

    handleReceivedFrame(rt);

    if (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(
        rt.annotation<senf::emu::annotations::WirelessModulation>().id).rate == 0) {
        handleOtherFrame(rt);
        return;
    }

    if (q.flags.frameCorrupt) {
        handleCorruptFrame(rt);
        return;
    }

    switch (rt->frameType()) {
    case 0x00:
    {
        senf::WLANPacket_MgtFrame wlanp (rt.next<senf::WLANPacket_MgtFrame>(senf::nothrow));
        if (wlanp) {
            handleManagementFrame(wlanp);
        }
    }
    break;
    case 0x01:
    {
        senf::WLANPacket_CtrlFrame wlanp (rt.next<senf::WLANPacket_CtrlFrame>(senf::nothrow));
        if (wlanp) {
            handleControlFrame(wlanp);
        }
    }
    break;
    default:
        handleOtherFrame(rt);
        break;
    }
}

prefix_ void FrameAnalyzer::request()
{
    if (startTime_ > senf::ClockService::clock_type(0) && configuration_.duration > senf::ClockService::clock_type(0)) {
        // we've been started
        if ((senf::scheduler::now() - startTime_) >= configuration_.duration) {
            senf::scheduler::terminate();
        }
    }

    senf::EthernetPacket eth (input());
    senf::emu::annotations::Quality const & q (eth.annotation<senf::emu::annotations::Quality>());

    handleReceivedFrame(eth);

    if (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(
        eth.annotation<senf::emu::annotations::WirelessModulation>().id).rate == 0) {
        handleOtherFrame(eth);
        return;
    }

    if (q.flags.frameCorrupt) {
        handleCorruptFrame(eth);
        return;
    }

    handleDataFrame(eth);
}


//////////////////////////////////////////////////

prefix_ void FrameAnalyzer::resetStats()
{
    received.v_reset();
    corrupt.v_reset();
    control.v_reset();
    management.v_reset();
    other.v_reset();
    data.v_reset();
    dataNonUDP.v_reset();
}

prefix_ void FrameAnalyzer::handleReceivedFrame(senf::Packet & pkt)
{
    received.v_analyze(pkt);
}

prefix_ void FrameAnalyzer::handleCorruptFrame(senf::Packet & pkt)
{
    corrupt.v_analyze(pkt);
}

prefix_ void FrameAnalyzer::handleOtherFrame(senf::Packet & pkt)
{
    other.v_analyze(pkt);
}

prefix_ void FrameAnalyzer::handleManagementFrame(senf::WLANPacket_MgtFrame & pkt)
{
    management.v_analyze(pkt);
}

prefix_ void FrameAnalyzer::handleControlFrame(senf::WLANPacket_CtrlFrame & pkt)
{
    control.v_analyze(pkt);
}

prefix_ void FrameAnalyzer::handleDataFrame(senf::EthernetPacket & eth)
{
    data.v_analyze( eth, eth.size());

    senf::IPv4Packet ip4 (eth.next<senf::IPv4Packet>(senf::nothrow));
    if (!ip4) {
        dataNonUDP.v_analyze( eth);
        return;
    }
    senf::UDPPacket udp (ip4.next<senf::UDPPacket>(senf::nothrow));
    if (!udp) {
        dataNonUDP.v_analyze( eth);
        return;
    }

    if (v_handleUDPPacket( eth, ip4, udp)) {
        if (configuration_.numPackets > 0) {
            if (++numPackets_ >= configuration_.numPackets) {
                senf::scheduler::terminate();
            }
        }
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
