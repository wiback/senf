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


#ifndef __ANALYZERBASE_HH__
#define __ANALYZERBASE_HH__

// Custom includes
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/PPI/Module.hh>
#include <senf/Ext/NetEmu/WLAN/AthSpectralScan.hh>
#include "Configuration.hh"

///////////////////////////////hh.p////////////////////////////////////////

class AnalyzerBase
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(AnalyzerBase);

public:
    senf::ppi::connector::PassiveInput<senf::RadiotapPacket> input;

    AnalyzerBase(Configuration const & configuration);

    std::string stats();
    bool startSpectralScan();

    senf::emu::AthSpectralScan & athSpectralScan();
    
protected:
    Configuration const & configuration_;

    virtual void v_timerInterval(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration) = 0;
    virtual void v_terminate(senf::ClockService::clock_type const & sessionDuration) = 0;

    virtual void v_80211FrameReceived(std::uint64_t tsft, unsigned frequency, signed rssi, unsigned rate, unsigned length, senf::RadiotapPacket & rt) = 0;

    void processSpectralEvent(std::uint64_t tsft, std::uint16_t frequency, unsigned numBins, void * spectralSample);
    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20 const &) = 0;
    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20_40 const &) = 0;
    virtual void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ath10k const &) = 0;

private:
    senf::scheduler::TimerEvent    timer_;
    senf::ClockService::clock_type startTime_;
    senf::ClockService::clock_type nextTimeout_;

    senf::emu::AthSpectralScan athSpectralScan_;

    unsigned spectralUnknownType_;
    unsigned pktData_;
    unsigned pktManagement_;
    unsigned pktControl_;
    unsigned pktOther_;
    unsigned pktTx_;
    unsigned pktExceptions_;
    unsigned pktFrequencyMismatch_;

    void request();
    void timerEvent();
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

