// $Id:$
//
// Copyright (C) 2010
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
//       Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

#include "AthSpectralScanner.hh"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::emu::AthSpectralScanner::AthSpectralScanner(std::string phyName, std::string debugFS) :
    frequency_(0),
    mismatch_(0),
    truncated_(0),
    event_("spectralScan_" + phyName, senf::membind( &AthSpectralScanner::process, this))
{
    ctlFile_  = debugFS + "/ieee80211/" + phyName + "/ath9k/spectral_scan_ctl";
    dataFile_ = debugFS + "/ieee80211/" + phyName + "/ath9k/spectral_scan0";
}

prefix_ senf::emu::AthSpectralScanner::~AthSpectralScanner()
{
    stop();
}

prefix_ bool senf::emu::AthSpectralScanner::enable()
{
    int fd;
    if ((fd = open( ctlFile_.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write( fd, "background", 10));
        close( fd);
    }
    if ((fd = open( ctlFile_.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write( fd, "trigger", 7));
        close( fd);
    }

    return true;
}

prefix_ void senf::emu::AthSpectralScanner::disable()
{
    int fd;
    if ((fd = open( ctlFile_.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write( fd, "disable", 7));
        close( fd);
    }
}

prefix_ bool senf::emu::AthSpectralScanner::valid()
    const
{
    return ::access(ctlFile_.c_str(), W_OK) == 0;
}

prefix_ std::string senf::emu::AthSpectralScanner::ctlFile()
    const
{
    return ctlFile_;
}

prefix_ unsigned senf::emu::AthSpectralScanner::mismatch()
    const
{
    return mismatch_;
}

prefix_ unsigned senf::emu::AthSpectralScanner::truncated()
    const
{
    return truncated_;
}

prefix_ bool senf::emu::AthSpectralScanner::start(unsigned frequency)
{
    if (!valid())
        return false;

    disable();

    int fd;
    if ((fd = open( dataFile_.c_str(), O_RDONLY|O_NONBLOCK)) == -1) {
        return false;
    }

    // flush any old samples
    fft_sample_ht20 samples[4];
    while (read(fd, samples, sizeof(samples)) > 0);

    handle_.SetFd(fd);
    event_.handle(handle_);
    event_.events( senf::scheduler::FdEvent::EV_READ);
    event_.enable();

    frequency_ = frequency;

    mismatch_ = 0;
    truncated_ = 0;
    signalLevel_.clear();

    enable();

    return true;
}

prefix_ bool senf::emu::AthSpectralScanner::stop(senf::StatisticsData * sd, unsigned thresh1000)
{
    disable();

    if (!handle_)
        return false;

    while (process(0));

    event_.disable();
    handle_.close();

    if (sd) {
        signalLevel_.data(*sd, thresh1000);
        signalLevel_.clear();
        return sd->valid();
    }

    return true;
}

prefix_ float senf::emu::AthSpectralScanner::computeSignalLevel(fft_sample_ht20 const & sample)
{
    std::uint16_t maxData = 0;
    std::uint64_t datasquaresum = 0;

    for (unsigned i = 0; i < SPECTRAL_HT20_NUM_BINS; i++) {
        std::uint16_t data (std::max(1, sample.data[i] << sample.max_exp));  // std::max to avoid NANs below
        datasquaresum += data * data;
        maxData = std::max (maxData, data);
    }

    // supposedly max_magnitude is the maximum among the samples, so we actually do not need to compute maxData ourselves
    // However, using max_magnitude returns wired results....

    return float(sample.noise) + float(sample.rssi) + 20.0f * log10f(float(maxData)) - log10f(float(datasquaresum)) * 10.0f;

    /*
    //
    // the old, more detailed computation. not needed for our purposes
    //
    senf::StatisticAccumulator<std::int32_t> signalLevel;
    for (unsigned i = 0; i < SPECTRAL_HT20_NUM_BINS-1; i++) {
        int data;
        data = sample.data[i] << sample.max_exp;
        if (data == 0)
            data = 1;
        signalLevel.accumulate(sample.noise + sample.rssi + 20 * log10f(data) - log10f(datasquaresum) * 10);
    }

    return signalLevel.max();
    */
}

prefix_ bool senf::emu::AthSpectralScanner::process(int fd)
{
    if (!handle_)
        return false;

    fft_sample_ht20 samples[4];
    size_t bytes;
    if ((bytes = read( handle_.fd(), samples, sizeof(samples))) > 0) {
        if ((bytes % sizeof(fft_sample_ht20)) > 0) {
            truncated_++;
        }
        for (unsigned n = 0; n < bytes / sizeof(fft_sample_ht20); n++) {
            if (samples[n].tlv.type == ATH_FFT_SAMPLE_HT20) {
                if (be16toh(samples[n].freq) != frequency_ / 1000) {
                    mismatch_++;
                    continue;
                }
                signalLevel_.accumulate(computeSignalLevel(samples[n]));
            }
        }
    }

    return bytes > 0;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
