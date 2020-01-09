// $Id$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fit.fraunhofer.de/license.html
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
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

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
