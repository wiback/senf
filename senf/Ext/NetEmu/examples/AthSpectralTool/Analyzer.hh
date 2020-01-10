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


#ifndef __ANALYZER_HH__
#define __ANALYZER_HH__

// Custom includes
#include "AnalyzerBase.hh"

///////////////////////////////hh.p////////////////////////////////////////

class Analyzer
    : public AnalyzerBase
{
public:
    Analyzer(Configuration const & configuration);
    ~Analyzer();
    
private:
    void v_timerInterval(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);
    void v_terminate(senf::ClockService::clock_type const & sessionDuration);
    void v_80211FrameReceived(std::uint64_t tsft, unsigned frequency, signed rssi, unsigned rate, unsigned length, senf::RadiotapPacket & rt);
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20 const &);
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20_40 const &);
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ath10k const &);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

