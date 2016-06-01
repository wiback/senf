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
     spectralEvent_("spectralScan_" + configuration.phyName, senf::membind( &AnalyzerBase::handleSpectralEvent, this)),
     pktData_(0),
     pktManagement_(0),
     pktControl_(0),
     pktOther_(0),
     pktTx_(0),
     pktExceptions_(0),
     pktFrequencyMismatch_(0),
     spectralSamples_(0),
     spectralUnknown_(0),
     spectralTruncated_(0),
     spectralFrequencyMismatch_(0)
{
    noroute(input);
    input.onRequest( &AnalyzerBase::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);

    // start the interval timer
    nextTimeout_ = startTime_ + configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);

    spectralPath_ = configuration_.debugFS + "/ieee80211/" + configuration_.phyName + "/ath9k";
    if (access(spectralPath_.c_str(), O_RDONLY) != 0)
        spectralPath_ = configuration_.debugFS + "/ieee80211/" + configuration_.phyName + "/ath10k";
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
            unsigned id (senf::emu::WLANModulationParameterRegistry::instance().parameterIdByMCS(
                              rtParser.mcs().mcsIndex(),
                              rtParser.mcs().bandwidth(),
                              rtParser.mcs().guardInterval() ));
            rate = senf::emu::WLANModulationParameterRegistry::instance().findModulationById(id).rate;
        }
        else if (rtParser.vhtPresent()) {
            // we need to extend the Modulation registry to support VHT...
            rate = 1000000;  // 1Gbps (dummy for now)
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

prefix_ void AnalyzerBase::handleSpectralEvent(int _dummy_)
{
    fft_sample_tlv tlv;

    if (read(spectralHandle_.fd(), &tlv, sizeof(tlv)) == sizeof(tlv)) {    
        switch (tlv.type) {
        case ATH_FFT_SAMPLE_HT20:
            {
                fft_sample_ht20 ht20;
                ht20.tlv = tlv;
                if (read(spectralHandle_.fd(), ((char*)&ht20) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) {
                    if (be16toh(ht20.freq) == configuration_.frequency) {
                        spectralSamples_++;
                        v_SpectralDataReceived(be64toh(ht20.tsf), be16toh(ht20.freq), ht20);
                    } else {
                        spectralFrequencyMismatch_++;
                    }
                } else {
                    spectralTruncated_++;
                }
            }
            break;
        case ATH_FFT_SAMPLE_HT20_40:
            {
                fft_sample_ht20_40 ht20_40;
                ht20_40.tlv = tlv;
                if (read(spectralHandle_.fd(), ((char*)&ht20_40) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) { 
                    if (be16toh(ht20_40.freq) == configuration_.frequency) {
                        spectralSamples_++;
                        v_SpectralDataReceived(be64toh(ht20_40.tsf), be16toh(ht20_40.freq), ht20_40);
                    } else {
                        spectralFrequencyMismatch_++;
                    }
                } else {
                    spectralTruncated_++;
                }
            }
            break;
        case ATH_FFT_SAMPLE_ATH10K:
            {
                char buf[1024]; // fft_sample_ath10k has avariable length, depending on the number bins
                fft_sample_ath10k *ath10k = (fft_sample_ath10k *) buf;
                ath10k->tlv = tlv;
                if (read(spectralHandle_.fd(), ((char*)ath10k) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) {
                    spectralSamples_++;
                    v_SpectralDataReceived(be64toh(ath10k->tsf), be16toh(ath10k->freq1), be16toh(ath10k->freq2), *ath10k, configuration_.spectralBins);
                } else {
                    spectralTruncated_++;
                }
            }
            break;
        default:
            spectralUnknown_++;
            break;
        }
    } else {
        spectralTruncated_++;
    }
}

prefix_ unsigned AnalyzerBase::spectralSetting( std::string option)
{
    int cfd;
    if ((cfd = open( (spectralPath_ + "/" + option).c_str(), O_RDONLY)) != -1) {
        char buf[128]; int rd;
        if ((rd = read(cfd, buf, sizeof((buf)))) > 0) {
            buf[rd] = 0;
            return atoi(buf);
        }
    }
    
    return unsigned(-1);
}

prefix_ bool AnalyzerBase::spectralSetting( std::string option, unsigned value)
{
    int cfd;
    if ((cfd = open( (spectralPath_ + "/" + option).c_str(), O_WRONLY)) != -1) {
        std::string tmp (senf::str(value));
        bool rtn (write(cfd, tmp.c_str(), tmp.size()) == signed(tmp.size()));
        close(cfd);
        return rtn;
    }

    return false;
}

prefix_ bool AnalyzerBase::startSpectralScan()
{
    std::string ctlFile_  (spectralPath_ + "/spectral_scan_ctl");
    std::string dataFile_ (spectralPath_ + "/spectral_scan0");


    if (spectralPath_.find("ath9k") != std::string::npos) {
        if (!spectralSetting( "spectral_period", configuration_.spectralPeriod))
            return false;
        if (!spectralSetting( "spectral_fft_period", configuration_.spectralFFTPeriod))
        return false;
        if (!spectralSetting( "spectral_short_repeat", configuration_.spectralShortRepeat))
            return false;
    } else {
        // ath10k
        if (!spectralSetting( "spectral_bins", configuration_.spectralBins))
            return false;
    }
    if (!spectralSetting( "spectral_count", configuration_.spectralCount))
        return false;

    int dfd;
    if ((dfd = open( dataFile_.c_str(), O_RDONLY|O_NONBLOCK)) == -1) {
        return false;
    }

    // flush any old samples
    char buf[4096];
    while (read(dfd, buf, sizeof(buf)) > 0);

    spectralHandle_.SetFd(dfd);
    spectralEvent_.handle(spectralHandle_);
    spectralEvent_.events( senf::scheduler::FdEvent::EV_READ);
    spectralEvent_.enable();

    int cfd;
    if ((cfd = open( ctlFile_.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write(cfd, "background", 10));
        close(cfd);
    }
    if ((cfd = open( ctlFile_.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write(cfd, "trigger", 7));
        close(cfd);
    }

    return true;
}

prefix_ std::string AnalyzerBase::stats()
{
    return "pktData " + senf::str(pktData_) +
        ", pktManagement " + senf::str(pktManagement_) +
        ", pktControl " + senf::str(pktControl_) +
        ", pktOther " + senf::str(pktOther_) +
        ", pktTx " + senf::str(pktTx_) +
        ", pktExceptions " + senf::str(pktExceptions_) +
        ", pktFrequencyMismatch " + senf::str(pktFrequencyMismatch_) +

        " --- spectralSamples " + senf::str(spectralSamples_) +
        ", spectralUnknown " + senf::str(spectralUnknown_) +
        ", spectralTruncated " + senf::str(spectralTruncated_) +
        ", spectralFrequencyMismatch " + senf::str(spectralFrequencyMismatch_);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
