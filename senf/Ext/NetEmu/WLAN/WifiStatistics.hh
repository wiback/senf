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

#ifndef HH_SENF_Ext_NetEmu_WLAN_WifiStatistics_
#define HH_SENF_Ext_NetEmu_WLAN_WifiStatistics_ 1

// Custom includes
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <senf/Socket/FileHandle.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Utils/Statistics.hh>

///////////////////////////////hh.p////////////////////////////////////////


namespace senf {
namespace emu {

    struct WifiStatisticsData {
        senf::StatisticsData signal;
        senf::StatisticsData signalNonData;
        senf::StatisticsData bitrate;
        senf::StatisticsData bitrateNonData;
        std::uint32_t total; 
        std::uint32_t totalBytes;
        std::uint32_t badFCS; 
        std::uint32_t badFCSBytes; 
        std::uint32_t rTx;
        std::uint32_t rTxBytes;
        std::uint32_t airTime;
        senf::ClockService::clock_type lastSeen;
        senf::MACAddress bssId;
        std::string ssId;
        std::string type;
        
        WifiStatisticsData() {
            total = totalBytes = badFCS = badFCSBytes = rTx = rTxBytes = airTime = 0;
            lastSeen = senf::ClockService::clock_type(0);
        };
    };

    typedef std::unordered_map<senf::MACAddress,WifiStatisticsData> WifiStatisticsMap;

    class WifiStatistics {
    public:
        WifiStatistics (std::string ifName, std::string debugFS = "/sys/kernel/debug");
        ~WifiStatistics();
        
        bool enable(bool on = true);
        bool pollStatistics(std::uint32_t tag, senf::ClockService::clock_type const & maxAge);
        bool pollStatisticsCSV(std::uint32_t tag, senf::ClockService::clock_type const & maxAge);
        WifiStatisticsMap const & map() const;
        ClockService::clock_type const & timestamp() const;
        std::uint32_t tag() const;
        std::uint32_t invalidEntries() const;
        std::uint32_t ioErrors() const;

        WifiStatisticsMap const & statisticsMap(std::uint32_t tag, senf::ClockService::clock_type const & maxAge);

    private:
        std::string debugFsPath_;
        WifiStatisticsMap map_;
        ClockService::clock_type timestamp_;
        std::uint32_t tag_;
        std::uint32_t invalidEntries_;
        std::uint32_t ioErrors_;
    };
    
}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "WifiStatistics.cci"
//#include "WifiStatistics.ct"
//#include "WifiStatistics.cti"
#endif
