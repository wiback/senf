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
        unsigned spectralSetting( std::string option);
        
        bool callback(AthSpectralScanCallback const & cb);
        void disable();
        void frequency(std::uint32_t freq);

        bool detected() const;

        void stats(std::ostream & os);
        
    private:
        UnixFileHandle spectralHandle_;
        senf::scheduler::FdEvent spectralEvent_;
        std::string spectralPath_;
        AthSpectralScanCallback callback_;
        std::uint32_t frequency_;

        unsigned spectralPeriod_;
        unsigned spectralFFTPeriod_;
        unsigned spectralShortRepeat_;
        unsigned spectralBins_;
        unsigned spectralCount_;

        unsigned spectralFrames_;
        unsigned spectralValidSamples_;
        unsigned spectralUnknown_;
        unsigned spectralTruncated_;
        unsigned spectralFrequencyMismatch_;
        
        void handleSpectralEvent(int fd);

        static void dummy(std::uint64_t tsft, std::uint16_t frequency, unsigned numBins, void * spectralSample) {};
    };

}}
///////////////////////////////hh.p////////////////////////////////////////
#endif

