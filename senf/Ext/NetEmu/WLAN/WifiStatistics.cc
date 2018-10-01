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
    : timestamp_ (senf::ClockService::clock_type(0)), tag_(0),
      invalidEntries_(0), ioErrors_(0)
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

prefix_ bool senf::emu::WifiStatistics::pollStatistics(std::uint32_t tag, senf::ClockService::clock_type const & maxAge)
{
    if (timestamp_ and (tag_ == tag))
        return true;
    
    map_.clear();
    timestamp_ = senf::ClockService::clock_type(0);

    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(debugFsPath_ + "stats", pt);

        for (auto const & v : pt) {
            WifiStatisticsData data;
            unsigned num (0);  // keep track of numer of parsed items (we need 8)
            for (auto const & it : v.second) {
                if (it.first == "signal") {
                    std::uint32_t sum, min, max, count;
                    std::uint64_t sum2;
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "sum2")
                            sum2 = boost::lexical_cast<std::uint64_t>(s.second.data());
                        if (s.first == "min")
                            min = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "max")
                            max = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "count")
                            count = boost::lexical_cast<std::uint32_t>(s.second.data());
                    }
                    data.signal = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                    num++;
                }
                else if (it.first == "signalNonData") {
                    // otional
                    std::uint32_t sum, min, max, count;
                    std::uint64_t sum2;
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "sum2")
                            sum2 = boost::lexical_cast<std::uint64_t>(s.second.data());
                        if (s.first == "min")
                            min = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "max")
                            max = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "count")
                            count = boost::lexical_cast<std::uint32_t>(s.second.data());
                    }
                    data.signalNonData = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "bitrate") {
                    // optional
                    std::uint32_t sum, min, max, count;
                    std::uint64_t sum2;
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "sum2")
                            sum2 = boost::lexical_cast<std::uint64_t>(s.second.data());
                        if (s.first == "min")
                            min = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "max")
                            max = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "count")
                            count = boost::lexical_cast<std::uint32_t>(s.second.data());
                    }
                    data.bitrate = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "bitrateNonData") {
                    // optional
                    std::uint32_t sum(0), min(0), max(0), count(0);
                    std::uint64_t sum2 (0);
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "sum2")
                            sum2 = boost::lexical_cast<std::uint64_t>(s.second.data());
                        if (s.first == "min")
                            min = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "max")
                            max = boost::lexical_cast<std::uint32_t>(s.second.data());
                        if (s.first == "count")
                            count = boost::lexical_cast<std::uint32_t>(s.second.data());
                    }
                    data.bitrateNonData = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "badFCS") {
                    data.badFCS = it.second.get<std::uint32_t>("");
                    num++;
                }
                else if (it.first == "badFCSBytes") {
                    data.badFCSBytes = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "rTx") {
                    data.rTx = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "rTxBytes") {
                    data.rTxBytes = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "total") {
                    data.total = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "totalBytes") {
                    data.totalBytes = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "airTime") {
                    data.airTime = boost::lexical_cast<std::uint32_t>(it.second.data());
                    num++;
                }
                else if (it.first == "bssId") {
                    // optional
                    data.bssId = senf::MACAddress::from_string(boost::lexical_cast<std::string>(it.second.data()));
                }
                else if (it.first == "ssId") {
                    // optional
                    data.ssId = boost::lexical_cast<std::string>(it.second.data());
                }
                else if (it.first == "type") {
                    // optional
                    data.type = boost::lexical_cast<std::string>(it.second.data());
                }
                else if (it.first == "lastSeen") {
                    // optional
                    data.lastSeen = senf::ClockService::milliseconds(boost::lexical_cast<std::uint32_t>(it.second.data()));
                }
            }
            if (num == 8) {
                if (data.totalBytes > 0 and data.lastSeen <= maxAge) {
                    // only add entries with had activity
                    map_.emplace(std::make_pair(senf::MACAddress::from_string(v.first), data));
                }
            } else {
                invalidEntries_++;
            }
        }
    } catch(std::exception const & ex) {
        ioErrors_++;
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

prefix_ std::uint32_t senf::emu::WifiStatistics::invalidEntries()
    const
{
    return invalidEntries_;
}

prefix_ std::uint32_t senf::emu::WifiStatistics::ioErrors()
    const
{
    return ioErrors_;
}

prefix_ senf::emu::WifiStatisticsMap const & senf::emu::WifiStatistics::statisticsMap(std::uint32_t tag, senf::ClockService::clock_type const & maxAge)
{
    pollStatistics(tag, maxAge);
    return map_;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
