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
     spectralUnknown_(0),
     spectralTruncated_(0)
{
    noroute(input);
    input.onRequest( &AnalyzerBase::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);

    // start the interval timer
    nextTimeout_ = startTime_ + configuration_.reportingInterval;
    timer_.timeout( nextTimeout_);
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
    }
    catch(...) {
        pktExceptions_++;
    };
    
    v_80211FrameReceived(rtParser.tsft(), rtPacket);
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
                    v_SpectralDataReceived(be64toh(ht20.tsf), ht20);
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
                    v_SpectralDataReceived(be64toh(ht20_40.tsf), ht20_40);
                } else {
                    spectralTruncated_++;
                }
            }
            break;
        case ATH_FFT_SAMPLE_ATH10K:
            {
                fft_sample_ath10k ath10k;
                ath10k.tlv = tlv;
                if (read(spectralHandle_.fd(), ((char*)&ath10k) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) {
                    v_SpectralDataReceived(be64toh(ath10k.tsf), ath10k);
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

prefix_ bool AnalyzerBase::startSpectralScan()
{
    std::string ctlFile_  = configuration_.debugFS + "/ieee80211/" + configuration_.phyName + "/ath9k/spectral_scan_ctl";
    std::string dataFile_ = configuration_.debugFS + "/ieee80211/" + configuration_.phyName + "/ath9k/spectral_scan0";
    
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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
