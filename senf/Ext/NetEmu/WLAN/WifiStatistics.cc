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

#include "WifiStatistics.hh"

// Custom includes
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <senf/Scheduler/Scheduler.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::emu::WifiStatistics::WifiStatistics(std::string ifName, std::string debugFS)
    : timestamp_ (senf::ClockService::clock_type(0)), tag_(0)
{
    debugFsPath_ = debugFS + "/wifi_statistics/" + ifName + "/";
}


prefix_ senf::emu::WifiStatistics::~WifiStatistics()
{
    enable(false);
}

prefix_ bool senf::emu::WifiStatistics::enable(bool on)
{
    std::string file (debugFsPath_ + "active");
    int fd;
    if ((fd = open( file.c_str(), O_WRONLY)) != -1) {
        senf::IGNORE(write( fd, on ? "1" : "0", 1));
        close( fd);
        return true;
    }
    
    return false;
}

prefix_ bool senf::emu::WifiStatistics::pollStatistics(std::uint32_t tag)
{
    if (timestamp_ and (tag_ == tag))
        return true;
    
    map_.clear();
    timestamp_ = senf::ClockService::clock_type(0);

    try {
        boost::filesystem::ifstream file;
        file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        file.open(debugFsPath_ + "stats", std::ios::in);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(file, pt);
        file.close();        

        unsigned num; // keep track of numer of parsed items (we need 4)
        for (boost::property_tree::ptree::value_type & v : pt) {
            senf::MACAddress mac(senf::MACAddress::from_string(v.first));
            map_.insert(std::make_pair(mac, WifiStatisticsData()));
            num = 0;
            for (auto it = v.second.begin(); it != v.second.end(); it++) {
                if (it->first == "signal") {
                    map_.find(mac)->second.signal = StatisticAccumulator<std::int64_t>(
                            it->second.get<std::int32_t>("sum"),
                            it->second.get<std::int64_t>("sum2"),
                            it->second.get<std::int32_t>("min"),
                            it->second.get<std::int32_t>("max"),
                            it->second.get<std::uint32_t>("count")).data();
                    num++;
                } else {
                    if (it->first == "badFCS") {
                        map_.find(mac)->second.badFCS = it->second.get<std::uint32_t>("");
                        num++;
                    }
                    else if (it->first == "rTx") {
                        map_.find(mac)->second.rTx = it->second.get<std::uint32_t>("");
                        num++;
                    }
                    else if (it->first == "total") {
                        map_.find(mac)->second.total = it->second.get<std::uint32_t>("");
                        num++;
                    }
                    else if (it->first == "totalBytes") {
                        map_.find(mac)->second.totalBytes = it->second.get<std::uint32_t>("");
                        num++;
                    }
                    else if (it->first == "bssId") {
                        // optional
                        map_.find(mac)->second.bssId = senf::MACAddress::from_string(it->second.get<std::string>(""));
                    }
                    else if (it->first == "ssId") {
                        // optional
                        map_.find(mac)->second.ssId = it->second.get<std::string>("");
                    }
                    else if (it->first == "type") {
                        // optional
                        map_.find(mac)->second.type = it->second.get<std::string>("");
                    }
                }
            }
            if (num != 5) {
                map_.clear();
                return false;
            }
        }
    } catch(...) {
        map_.clear();
        return false;
    }

    tag_ = tag;
    timestamp_ = senf::scheduler::now();
    return true;
}

prefix_ senf::emu::WifiStatisticsMap const & senf::emu::WifiStatistics::map()
    const
{
    return map_;
}

prefix_ senf::ClockService::clock_type const & senf::emu::WifiStatistics::timestamp()
    const
{
    return timestamp_;
}

prefix_ std::uint32_t senf::emu::WifiStatistics::tag()
    const
{
    return tag_;
}

prefix_ senf::emu::WifiStatisticsMap const & senf::emu::WifiStatistics::statisticsMap(std::uint32_t tag)
{
    pollStatistics(tag);
    return map_;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
