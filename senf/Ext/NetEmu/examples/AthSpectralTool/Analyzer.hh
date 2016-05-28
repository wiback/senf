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
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, fft_sample_ht20 const &);
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, fft_sample_ht20_40 const &);
    void v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency1, unsigned frequency2, fft_sample_ath10k const &);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif

