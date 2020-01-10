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


#ifndef HH_SENF_Ext_NetEmu_WLAN_AthSpectralScan_
#define HH_SENF_Ext_NetEmu_WLAN_AthSpectralScan_ 1

// Custom includes
#include <senf/Socket/FileHandle.hh>
#include <senf/Scheduler/FdEvent.hh>
#include "spectral_common.h"

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    class UnixFileHandle
        : public senf::FileHandle
    {
    public:
        UnixFileHandle() : senf::FileHandle(std::unique_ptr<senf::FileBody>(new senf::FileBody)) {
        }
        
        void SetFd(int fd_) {
            fd(fd_);
        }
    };
    
    class AthSpectralScan
    {
    public:
        typedef std::function<void (std::uint64_t tsft, std::uint16_t frequency, unsigned numBins, void * spectralSample)> AthSpectralScanCallback;
        
        AthSpectralScan(std::string phyName);
        ~AthSpectralScan();

        bool spectralSetting( std::string option, std::string value);
        bool spectralSetting( std::string option, unsigned value);
        unsigned spectralSetting( std::string option) const;

        bool callback(AthSpectralScanCallback const & cb);
        void disable();
        void frequency(std::uint32_t freq, std::uint32_t bandwidth, std::int32_t offset = 0);

        bool detected() const;
        bool is_ath9k() const;
        bool is_ath10k() const;

        void stats(std::ostream & os);

        unsigned spectralPeriod() const;
        unsigned spectralFFTPeriod() const;
        unsigned spectralShortRepeat() const;
        unsigned spectralCount() const;
        unsigned spectralBins() const;

        bool spectralPeriod(unsigned v);
        bool spectralFFTPeriod(unsigned v);
        bool spectralShortRepeat(unsigned v);
        bool spectralCount(unsigned v);
        bool spectralBins(unsigned v);
        
    private:
        UnixFileHandle spectralHandle_;
        senf::scheduler::FdEvent spectralEvent_;
        std::string spectralPath_;
        AthSpectralScanCallback callback_;
        std::uint32_t frequency_;
        std::int32_t frequencyOffset_;

        unsigned spectralFrames_;
        unsigned spectralValidSamples_;
        unsigned spectralUnknown_;
        unsigned spectralTruncated_;
        unsigned spectralFrequencyMismatch_;
        
        void handleSpectralEvent(int fd);
    };

}}
///////////////////////////////hh.p////////////////////////////////////////
#endif

