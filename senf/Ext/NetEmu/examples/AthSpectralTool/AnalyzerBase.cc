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
//     Mathias Kretschmer <mathias.kretschmer@fit.fraunhofer.de>

#include "AnalyzerBase.hh"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ AnalyzerBase::AnalyzerBase(Configuration const & configuration)
   : configuration_(configuration),
     timer_("intervalTimer", senf::membind( &AnalyzerBase::timerEvent, this)),
     startTime_(senf::ClockService::now()),
     nextTimeout_(senf::ClockService::clock_type(0)),
     athSpectralScan_(configuration.phyName),
     spectralUnknownType_(0),
     pktData_(0),
     pktManagement_(0),
     pktControl_(0),
     pktOther_(0),
     pktTx_(0),
     pktExceptions_(0),
     pktFrequencyMismatch_(0)
{
    noroute(input);
    input.onRequest( &AnalyzerBase::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);

    // start the interval timer
    nextTimeout_ = startTime_ + configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);
}

prefix_ senf::emu::AthSpectralScan & AnalyzerBase::athSpectralScan()
{
    return athSpectralScan_;
}

prefix_ void AnalyzerBase::timerEvent()
{
    // get current time
    senf::ClockService::clock_type current (senf::ClockService::now());
    // call report with current time and the actual interval duration
    v_timerInterval( current - startTime_, configuration_.reportingInterval + (current - nextTimeout_));
    // restart timer fixing any potential scheduling issues
    nextTimeout_ += configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);

    if (configuration_.duration and ((current - startTime_) >= configuration_.duration)) {
        v_terminate(current - startTime_);
        senf::scheduler::terminate();
    }
}

prefix_ bool AnalyzerBase::startSpectralScan()
{
    if ( !athSpectralScan_.spectralPeriod(configuration_.spectralPeriod) or
         !athSpectralScan_.spectralFFTPeriod(configuration_.spectralFFTPeriod) or
         !athSpectralScan_.spectralShortRepeat(configuration_.spectralShortRepeat) or
         !athSpectralScan_.spectralBins(configuration_.spectralBins) or
         !athSpectralScan_.spectralCount(configuration_.spectralCount))
        return false;

    bool rtn = athSpectralScan_.callback(
               std::bind(&AnalyzerBase::processSpectralEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    athSpectralScan_.frequency(configuration_.frequency, 0, 0);
    return rtn;
}

prefix_ void AnalyzerBase::request()
{
    senf::RadiotapPacket rtPacket (input());
    senf::RadiotapPacket::Parser rtParser (rtPacket.parser());

    if (!rtParser.tsftPresent()) {
        // ignore Tx Frames
        pktTx_++;
        return;
    }

    unsigned freq (0);
    unsigned rate(0);
    signed rssi(0);
    unsigned length(0);
    
    try {        
        senf::WLANPacket_DataFrame dpkt (rtPacket.next<senf::WLANPacket_DataFrame>(senf::nothrow));
        if (dpkt) {
            pktData_++;
        }
        else {
            senf::WLANPacket_MgtFrame mpkt (rtPacket.next<senf::WLANPacket_MgtFrame>(senf::nothrow));
            if (mpkt) {
                pktManagement_++;
            }
            else {
                senf::WLANPacket_CtrlFrame cpkt (rtPacket.next<senf::WLANPacket_CtrlFrame>(senf::nothrow));
                if (cpkt) {
                    pktControl_++;
                }
                else {
                    pktOther_++;
                }
            }
        }

        // determine frequency
        if (rtParser.channelOptionsPresent()) {
            freq = rtParser.channelOptions().freq();
        }
        
        // determine data rate
        if (rtParser.mcsPresent()) {
            unsigned id (senf::emu::WLANModulationParameterRegistry::instance().parameterIdByMCS_HT(
                              rtParser.mcs().mcsIndex(),
                              rtParser.mcs().bandwidth(),
                              rtParser.mcs().guardInterval() ));
            rate = senf::emu::WLANModulationParameterRegistry::instance().findModulationById(id).rate;
        }
        else if (rtParser.vhtPresent()) {
            // we need to extend the Modulation registry to support VHT...
            unsigned id (senf::emu::WLANModulationParameterRegistry::instance().parameterIdByMCS_VHT(
                rtParser.vht().mcs_user0(),
                rtParser.vht().nss_user0(),
                rtParser.bandwidth(),
                rtParser.vht().guardInterval() ));
                        
            rate = senf::emu::WLANModulationParameterRegistry::instance().findModulationById(id).rate;
        }
        else if (rtParser.ratePresent()) {
            rate = rtParser.rate() * 500;
        }
        
        rssi = short(rtParser.dbmAntennaSignal());
        length = rtPacket.size() - rtParser.length();   
    }
    catch(...) {
        pktExceptions_++;
    };

    if (freq == configuration_.frequency)
        v_80211FrameReceived(rtParser.tsft(), freq, rssi, rate, length, rtPacket);
    else
        pktFrequencyMismatch_++;
}

prefix_ void AnalyzerBase::processSpectralEvent(std::uint64_t tsft, std::uint16_t frequency, unsigned numBins, void * sample)
{
    switch (((fft_sample_tlv*) sample)->type) {
    case ATH_FFT_SAMPLE_HT20:
        v_SpectralDataReceived(tsft, frequency, numBins,  *((fft_sample_ht20 *) sample));
        break;
    case ATH_FFT_SAMPLE_HT20_40:
        v_SpectralDataReceived(tsft, frequency, numBins,  *((fft_sample_ht20_40 *) sample));
        break;
    case ATH_FFT_SAMPLE_ATH10K:
        v_SpectralDataReceived(tsft, frequency, numBins,  *((fft_sample_ath10k *) sample));
        break;
    default:
        spectralUnknownType_++;
        break;
    }
}

prefix_ std::string AnalyzerBase::stats()
{
    std::stringstream ss;
    athSpectralScan_.stats(ss);
    
    return "pktData " + senf::str(pktData_) +
        ", pktManagement " + senf::str(pktManagement_) +
        ", pktControl " + senf::str(pktControl_) +
        ", pktOther " + senf::str(pktOther_) +
        ", pktTx " + senf::str(pktTx_) +
        ", pktExceptions " + senf::str(pktExceptions_) +
        ", pktFrequencyMismatch " + senf::str(pktFrequencyMismatch_) +

        " --- " + senf::str(ss) + ", spectralUnknown " + senf::str(spectralUnknownType_);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
