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


#include "FrameAnalyzer.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>
#include "ChannelAnalyzer.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ FrameAnalyzer::FrameAnalyzer(Configuration const & configuration)
    : configuration_(configuration),
      timer_("reportingTimer", senf::membind( &FrameAnalyzer::timerEvent, this)),
      initWait_("maxWaitTimer", senf::membind( &FrameAnalyzer::initWaitEvent, this)),
      startTime_(senf::ClockService::milliseconds(0)),
      nextTimeout_(senf::ClockService::milliseconds(0)),
      numPackets_(0)
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
        return;
    }

    if (++numPackets_ == 1) {
        resetStartTime();
    }
    
    all.update(ap);
    
    if (ap->corrupt()) {
        corrupt.update(ap);
        return;
    }

    if (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate == 0) {
        norate.update(ap);
        return;
    }
    
    senf::WLANPacket_MgtFrame const & wlanm (ap.next<senf::WLANPacket_MgtFrame>(senf::nothrow));
    if (wlanm) {
        mngt.update(ap);
        return;
    }

    senf::WLANPacket_CtrlFrame const & wlanc (ap.next<senf::WLANPacket_CtrlFrame>(senf::nothrow));
    if (wlanc) {
        ctrl.update(ap);
        return;
    }

    // nonQoSData WLAN frames
    senf::WLANPacket_DataFrame const & dataf (ap.next<senf::WLANPacket_DataFrame>(senf::nothrow));
    if (dataf) {
        data.update(ap);
        return;
    }

    // QoS Data as Ethernet
    senf::EthernetPacket const & eth (ap.next<senf::EthernetPacket>(senf::nothrow));
    if (eth) {
        data.update(ap);
        return;
    }

    unknown.update(ap);
}

prefix_ void FrameAnalyzer::report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration)
{
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t All     "; all.dump(std::cout, timestamp)     ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t Data    "; data.dump(std::cout, timestamp)    ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t Mngt    "; mngt.dump(std::cout, timestamp)    ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t Ctrl    "; ctrl.dump(std::cout, timestamp)    ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t NoRate  "; norate.dump(std::cout, timestamp)  ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t Unknown "; unknown.dump(std::cout, timestamp) ; std::cout << std::endl;
    std::cout << senf::ClockService::in_milliseconds(timestamp) << "\t Corrupt "; corrupt.dump(std::cout, timestamp) ; std::cout << std::endl;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
