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


#ifndef HH_SENF_Packets_80211Bundle_MCSInfo_
#define HH_SENF_Packets_80211Bundle_MCSInfo_ 1

// Custom includes
#include <string>
#include <vector>
#include <boost/cstdint.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    // http://mcsindex.com/
    struct WLAN_MCSInfo
    {
        static constexpr unsigned NUM_STREAMS   = 4;
        static constexpr unsigned NUM_HT_INDEX  = 8;
        static constexpr unsigned NUM_VHT_INDEX = 10;
        static constexpr unsigned MAX_INDEX     = 12;
        
        struct Info
        {
            std::string    coding;
            unsigned       rate[8];
            short          rssi[4];
            std::uint8_t index;
            std::uint8_t streams;
        };

        static std::vector<Info> getInfos();
        static unsigned getRate(std::uint8_t mcsIndex, unsigned bandwidth, bool shortGI);
        static unsigned getRate(std::uint8_t index, std::uint8_t streams, unsigned bandwidth, bool shortGI);

        static std::uint8_t toHTIndex(std::uint8_t index, std::uint8_t streams);
        static std::pair<std::uint8_t,std::uint8_t> fromHTIndex(std::uint8_t mcsIndex);
        static std::uint8_t toBandwidthIndex(unsigned bandwidth, bool shortGI = false);
        static unsigned fromBandwidthIndex(std::uint8_t bandwidthIndex);

    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MCSInfo.cci"
//#include "MCSInfo.ct"
//#include "MCSInfo.cti"
#endif
