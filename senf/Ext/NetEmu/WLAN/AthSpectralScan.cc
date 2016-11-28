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

#include "AthSpectralScan.hh"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/String.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////


prefix_ senf::emu::AthSpectralScan::AthSpectralScan(std::string phyName)
   : spectralEvent_("spectralScan_" + phyName, senf::membind( &senf::emu::AthSpectralScan::handleSpectralEvent, this)),
     callback_(dummy),
     frequency_(0),
     spectralPeriod_(0x08),
     spectralFFTPeriod_(0x02),
     spectralShortRepeat_(true),
     spectralBins_(64),
     spectralCount_(8),
     spectralFrames_(0),
     spectralValidSamples_(0),
     spectralUnknown_(0),
     spectralTruncated_(0),
     spectralFrequencyMismatch_(0)
{
    spectralPath_ = "/sys/kernel/debug/ieee80211/" + phyName + "/ath9k";
    if (access(spectralPath_.c_str(), O_RDONLY) != 0)
        spectralPath_ = "/sys/kernel/debug/ieee80211/" + phyName + "/ath10k";
}

prefix_ senf::emu::AthSpectralScan::~AthSpectralScan()
{
    disable();
}

prefix_ bool senf::emu::AthSpectralScan::detected()
    const
{
    return access(spectralPath_.c_str(), O_RDONLY) == 0;
}

prefix_ void senf::emu::AthSpectralScan::disable()
{
    spectralSetting("spectral_scan_ctl", "disable");
    frequency_ = 0;
    callback_ = dummy;
    
    if(spectralHandle_) {
        spectralEvent_.disable();
        // flush any remaining samples
        char buf[4096];
        while (read(spectralHandle_.fd(), buf, sizeof(buf)) > 0);
        spectralHandle_.close();
    }
}

prefix_ void senf::emu::AthSpectralScan::frequency(std::uint32_t freq)
{
    if (!spectralHandle_) {
        frequency_ = 0;
        return;
    }

    // be tolerant, if we have been given kHz :)
    if (freq > 10000)
        freq /= 1000;
    
    frequency_ = freq;
}

prefix_ void senf::emu::AthSpectralScan::handleSpectralEvent(int _dummy_)
{
    unsigned burst(1024);

    while(burst-- > 0) {
        spectralFrames_++;    
        fft_sample_tlv tlv;
        size_t rd;
        if ((rd = read(spectralHandle_.fd(), &tlv, sizeof(tlv))) == sizeof(tlv)) {    
            switch (tlv.type) {
            case ATH_FFT_SAMPLE_HT20:
                {
                    fft_sample_ht20 ht20;
                    ht20.tlv = tlv;
                    if (read(spectralHandle_.fd(), ((char*)&ht20) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) {
                        if (be16toh(ht20.freq) == frequency_) {
                            spectralValidSamples_++;
                            callback_(be64toh(ht20.tsf), be16toh(ht20.freq), SPECTRAL_HT20_NUM_BINS, &ht20);
                        } else {
                            spectralFrequencyMismatch_++;
                        }
                    } else {
                        spectralTruncated_++;
                        burst = 0;
                    }
                }
                break;
            case ATH_FFT_SAMPLE_HT20_40:
                {
                    fft_sample_ht20_40 ht20_40;
                    ht20_40.tlv = tlv;
                    if (read(spectralHandle_.fd(), ((char*)&ht20_40) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) { 
                        if (be16toh(ht20_40.freq) == frequency_) {
                            spectralValidSamples_++;
                            callback_(be64toh(ht20_40.tsf), be16toh(ht20_40.freq), SPECTRAL_HT20_40_NUM_BINS, &ht20_40);
                        } else {
                            spectralFrequencyMismatch_++;
                        }
                    } else {
                        spectralTruncated_++;
                        burst = 0;
                    }
                }
                break;
            case ATH_FFT_SAMPLE_ATH10K:
                {
                    char buf[1024]; // fft_sample_ath10k has avariable length, depending on the number bins
                    fft_sample_ath10k *ath10k = (fft_sample_ath10k *) buf;
                    ath10k->tlv = tlv;
                    if (read(spectralHandle_.fd(), ((char*)ath10k) + sizeof(tlv), be16toh(tlv.length)) == be16toh(tlv.length)) {
                        spectralValidSamples_++;
                        callback_(be64toh(ath10k->tsf), be16toh(ath10k->freq1), spectralBins_, ath10k);
                    } else {
                        spectralTruncated_++;
                        burst = 0;
                    }
                }
                break;
            default:
                spectralUnknown_++;
                break;
            }
        } else {
            if (rd > 0) {
                spectralTruncated_++;
            }
            burst = 0;
        }
    }
}

prefix_ unsigned senf::emu::AthSpectralScan::spectralSetting( std::string option)
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

prefix_ bool senf::emu::AthSpectralScan::spectralSetting( std::string option, std::string value)
{
    int cfd;
    if ((cfd = open( (spectralPath_ + "/" + option).c_str(), O_WRONLY)) != -1) {
        bool rtn (write(cfd, value.c_str(), value.size()) == signed(value.size()));
        close(cfd);
        return rtn;
    }
    
    return false;
}

prefix_ bool senf::emu::AthSpectralScan::spectralSetting( std::string option, unsigned value)
{
    return spectralSetting(option, senf::str(value));
}

prefix_ bool senf::emu::AthSpectralScan::callback(AthSpectralScanCallback const & cb)
{
    if (spectralEvent_.enabled()) {
        disable();
    }
    
    if (spectralPath_.find("ath9k") != std::string::npos) {
        if (!spectralSetting( "spectral_period", spectralPeriod_))
            return false;
        if (!spectralSetting( "spectral_fft_period", spectralFFTPeriod_))
        return false;
        if (!spectralSetting( "spectral_short_repeat", spectralShortRepeat_))
            return false;
    } else {
        // ath10k
        if (!spectralSetting( "spectral_bins", spectralBins_))
            return false;
    }
    if (!spectralSetting( "spectral_count", spectralCount_))
        return false;

    int dfd;
    if ((dfd = open((spectralPath_ + "/spectral_scan0").c_str(), O_RDONLY|O_NONBLOCK)) == -1) {
        return false;
    }
    
    spectralHandle_.SetFd(dfd);
    spectralEvent_.handle(spectralHandle_);
    spectralEvent_.events( senf::scheduler::FdEvent::EV_READ);
    spectralEvent_.enable();

    spectralSetting("spectral_scan_ctl", "background");
    spectralSetting("spectral_scan_ctl", "trigger");

    spectralFrames_ = spectralValidSamples_ =  spectralFrequencyMismatch_ = spectralTruncated_ = spectralUnknown_ = 0;

    callback_ = cb;
    
    return true;
}

prefix_ void senf::emu::AthSpectralScan::stats(std::ostream & os)
{
    os << (spectralEvent_.enabled() ? "enabled: " : "disabled: ") << "fd " << spectralHandle_.fd()
       << ", frames total " << spectralFrames_ << ", valid samples " << spectralValidSamples_
       << ", frequency mismatch " << spectralFrequencyMismatch_
       << ", truncated " << spectralTruncated_ << ", unknown type " << spectralUnknown_ << std::endl;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
