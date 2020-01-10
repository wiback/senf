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


/** \file
    \brief TSFT Histogram header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_TSFTHistogram_
#define HH_SENF_Ext_NetEmu_WLAN_TSFTHistogram_ 1

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Scheduler/ClockService.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class TSFTHistogram 
    {
    public:
        senf::console::ScopedDirectory<> dir;

        TSFTHistogram();
        void update(senf::MACAddress const & src, boost::uint64_t const & tsft);
        bool start(senf::MACAddress const & src);
        bool start();
        bool enabled() const {
            return enabled_;
        }
        void dump(std::ostream & os) const;

    private:
        // we want a sorted map, even if it might be slightly slower - debugging only !
        typedef std::map<boost::uint32_t,boost::uint32_t> TSFTMap;
        TSFTMap tsftMap_;
        boost::uint64_t lastTSFT_;
        bool tsftMapInitialized_;
        senf::ClockService::clock_type startTime_;
        senf::ClockService::clock_type duration_;
        senf::MACAddress src_;
        bool enabled_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TSFTHistogram.cci"
//#include "TSFTHistogram.ct"
//#include "TSFTHistogram.cti"
#endif
