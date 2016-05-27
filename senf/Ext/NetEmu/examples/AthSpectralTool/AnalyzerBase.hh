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

#ifndef __ANALYZERBASE_HH__
#define __ANALYZERBASE_HH__

// Custom includes
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/PPI/Module.hh>
#include "Configuration.hh"
#include "spectral_common.h"

///////////////////////////////hh.p////////////////////////////////////////

class UnixFileHandle
    : public senf::FileHandle
{
public:
    UnixFileHandle() : senf::FileHandle(std::auto_ptr<senf::FileBody>(new senf::FileBody)) {
    }
    
    void SetFd(int fd_) {
        fd(fd_);
    };
};

class AnalyzerBase
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(AnalyzerBase);

public:
    senf::ppi::connector::PassiveInput<senf::RadiotapPacket> input;

    AnalyzerBase(Configuration const & configuration);

    bool startSpectralScan();
    bool spectralSetting( std::string option, unsigned value);

    std::string stats();
    
protected:
    Configuration const & configuration_;

    virtual void v_timerInterval(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration) = 0;

    virtual void v_80211FrameReceived(std::uint64_t tsft, unsigned frequency, signed rssi, unsigned rate, unsigned length, senf::RadiotapPacket & rt) = 0;

    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, fft_sample_ht20 const &) = 0;
    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, fft_sample_ht20_40 const &) = 0;
    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency1, unsigned frequency2, fft_sample_ath10k const &) = 0;
    
private:
    senf::scheduler::TimerEvent    timer_;
    senf::ClockService::clock_type startTime_;
    senf::ClockService::clock_type nextTimeout_;
    // spectral stuff
    UnixFileHandle spectralHandle_;
    senf::scheduler::FdEvent spectralEvent_;
    std::string spectralPath_;
    
    unsigned pktData_;
    unsigned pktManagement_;
    unsigned pktControl_;
    unsigned pktOther_;
    unsigned pktTx_;
    unsigned pktExceptions_;
    unsigned pktFrequencyMismatch_;
    unsigned spectralSamples_;
    unsigned spectralUnknown_;
    unsigned spectralTruncated_;
    unsigned spectralFrequencyMismatch_;
    
    void request();
    void handleSpectralEvent(int fd);
    void timerEvent();
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

