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

#include "Analyzer.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/spectral_common.h>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ Analyzer::Analyzer(Configuration const & configuration)
   : AnalyzerBase(configuration)
{
}

prefix_ Analyzer::~Analyzer()
{
}

prefix_ void Analyzer::v_terminate(senf::ClockService::clock_type const & sessionDuration)
{
    SENF_LOG((senf::log::MESSAGE)("session terminated after " << senf::ClockService::in_seconds(sessionDuration) << "s") );
}

prefix_ void Analyzer::v_timerInterval(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration)
{
    SENF_LOG((senf::log::MESSAGE)("timestamp " << senf::ClockService::in_milliseconds(timestamp)
                                  << "ms, actualIntervalDuration " << senf::ClockService::in_milliseconds(actualDuration) << "ms: "
                                  << "Some message here...") );
}

prefix_ void Analyzer::v_80211FrameReceived(std::uint64_t tsft, unsigned frequency, signed rssi, unsigned rate, unsigned length, senf::RadiotapPacket & rt)
{
    // this parses the Radiotap packet to get the Source Mac Address
    senf::MACAddress srcAddress (senf::MACAddress::None);
    try {
        senf::WLANPacket_DataFrame data (rt.next<senf::WLANPacket_DataFrame>(senf::nothrow));
        if (data && (data.size() >= senf::WLANPacket_DataFrameParser::init_bytes)) {
            srcAddress = data->sourceAddress();
        } else {
            senf::WLANPacket_MgtFrame mgt (rt.next<senf::WLANPacket_MgtFrame>(senf::nothrow));
            if (mgt && (mgt.size() >= senf::WLANPacket_MgtFrameParser::init_bytes)) {
                srcAddress = mgt->sourceAddress();
            } else {
                senf::WLANPacket_CtrlFrame ctrl (rt.next<senf::WLANPacket_CtrlFrame>(senf::nothrow));
                if (ctrl && ctrl->is_rts() && (ctrl.size() >= senf::WLANPacket_CtrlFrameParser::init_bytes)) {
                    srcAddress = ctrl->sourceAddress();
                }
            }
        }
    } catch (...) {
        // catch any possible exception here.
        // If we catch one, we assume that we failed to extract a valid/useful source address
        SENF_LOG( (senf::log::MESSAGE) ("Couldn't parse the source address from Radio packet") );
        srcAddress = senf::MACAddress::None;
    }

    // rough estimate without preamble, padding, FCS, etc !!!
    senf::ClockService::clock_type airtime (rate ? (senf::ClockService::microseconds((length * 8 * 1000) / rate)) : 0);

    SENF_LOG((senf::log::MESSAGE) ("tsft " << std::hex << tsft << std::dec << " 80211 frame received.Freqency " << frequency
            << ", rssi " << rssi << ", rate " << rate << ", length " << length
            << ", airtime " << senf::ClockService::in_microseconds(airtime) << "us") );
}

prefix_ void Analyzer::v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20 const & sample)
{
    SENF_LOG((senf::log::MESSAGE) ("tsft " << std::hex << tsft << std::dec << " Spectral samples (HT20) received on frequency " << frequency) );
}

prefix_ void Analyzer::v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ht20_40 const & sample)
{
    SENF_LOG((senf::log::MESSAGE) ("tsft " << std::hex << tsft << std::dec << " Spectral samples (HT20_40) received on frequency " << frequency) );
}

prefix_ void Analyzer::v_SpectralDataReceived(std::uint64_t tsft, unsigned frequency, unsigned bins, fft_sample_ath10k const & sample)
{
    SENF_LOG((senf::log::MESSAGE) ("tsft " << std::hex << tsft << std::dec << " Spectral samples (ATH10K) received on frequency " << frequency << ", bins " << bins) );
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
