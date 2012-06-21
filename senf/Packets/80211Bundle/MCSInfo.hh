// $Id$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
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
//   Thorsten Horstmann <tho@berlios.de>

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
        struct Info
        {
            unsigned char index;
            unsigned char streams;
            std::string   coding;
            unsigned      rate[4];
            short         rssi[2];
        };

        static std::vector<Info> getInfos();
        static Info const & getInfo(boost::uint8_t index);
        static unsigned getRate(boost::uint8_t mcsIndex, unsigned bandwidth, bool shortGI);
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MCSInfo.cci"
//#include "MCSInfo.ct"
//#include "MCSInfo.cti"
#endif
