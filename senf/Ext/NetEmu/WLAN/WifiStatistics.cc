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
#include <boost/algorithm/string.hpp>
#include <senf/Scheduler/Scheduler.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////


prefix_ senf::mmapFile::mmapFile(std::string const & fname)
    : fd_(-1), begin_(NULL), end_(NULL), next_(NULL), buffer_(NULL)
{
    if (!fname.empty()) {
        if (!open(fname)) {
            throw std::exception();
        }
    }
}

prefix_ senf::mmapFile::~mmapFile()
{
    if (fd_ != -1)
        close(fd_);
    if (buffer_)
        delete buffer_;
}

prefix_ void *senf::mmapFile::open(std::string const & fname)
{
    fd_ = ::open(fname.c_str(), O_RDONLY);
    if (fd_ < 0)
        return NULL;

    if (fstat(fd_, &stat_) < 0) {
        close (fd_); fd_ = -1;
        return NULL;
    }
    
    begin_ = (std::uint8_t *) mmap(0, stat_.st_size, PROT_READ, MAP_SHARED, fd_, 0);
    if (begin_ == MAP_FAILED) {
        // mmap failed, fall back to a convential read()
        size_t bsize (stat_.st_size > 0 ? stat_.st_size : 32768);
        buffer_ = new std::uint8_t[bsize];
        if ((bsize = ::read(fd_, buffer_, bsize)) <= 0) {
            close (fd_); fd_ = -1;
            delete buffer_; buffer_ = NULL;
            return NULL;
        }
        begin_ = buffer_;
        end_ = begin_ + bsize;
    } else {
        end_ = begin_ + stat_.st_size;
    }

    next_ = begin_;
    return begin_;
}

prefix_ void *senf::mmapFile::begin()
    const
{
    return begin_;
}

prefix_ void *senf::mmapFile::end()
    const
{
    return end_;
}

prefix_ bool senf::mmapFile::eof()
    const
{
    return next_ >= end_;
}


prefix_ off_t senf::mmapFile::size()
    const
{
    return off_t(end_ - begin_);
}


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
                    std::int32_t sum(0), min(0), max(0);
                    std::uint32_t count(0);
                    std::uint64_t sum2 (0);
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = std::stoi(s.second.data());
                        if (s.first == "sum2")
                            sum2 = std::stoull(s.second.data());
                        if (s.first == "min")
                            min = std::stoi(s.second.data());
                        if (s.first == "max")
                            max = std::stoi(s.second.data());
                        if (s.first == "count")
                            count = std::stoul(s.second.data());
                    }
                    data.signal = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                    num++;
                }
                else if (it.first == "signalNonData") {
                    // optional
                    std::int32_t sum(0), min(0), max(0);
                    std::uint32_t count(0);
                    std::uint64_t sum2 (0);
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = std::stoi(s.second.data());
                        if (s.first == "sum2")
                            sum2 = std::stoull(s.second.data());
                        if (s.first == "min")
                            min = std::stoi(s.second.data());
                        if (s.first == "max")
                            max = std::stoi(s.second.data());
                        if (s.first == "count")
                            count = std::stoul(s.second.data());
                    }
                    data.signalNonData = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "bitrate") {
                    // optional
                    std::uint32_t sum(0), min(0), max(0), count(0);
                    std::uint64_t sum2 (0);
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = std::stoul(s.second.data());
                        if (s.first == "sum2")
                            sum2 = std::stoull(s.second.data());
                        if (s.first == "min")
                            min = std::stoul(s.second.data());
                        if (s.first == "max")
                            max = std::stoul(s.second.data());
                        if (s.first == "count")
                            count = std::stoul(s.second.data());
                    }
                    data.bitrate = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "bitrateNonData") {
                    // optional
                    std::uint32_t sum(0), min(0), max(0), count(0);
                    std::uint64_t sum2 (0);
                    for (auto const & s : it.second) {
                        if (s.first == "sum")
                            sum = std::stoul(s.second.data());
                        if (s.first == "sum2")
                            sum2 = std::stoull(s.second.data());
                        if (s.first == "min")
                            min = std::stoul(s.second.data());
                        if (s.first == "max")
                            max = std::stoul(s.second.data());
                        if (s.first == "count")
                            count = std::stoul(s.second.data());
                    }
                    data.bitrateNonData = StatisticAccumulator<std::int64_t>(sum, sum2, min, max, count).data();
                }
                else if (it.first == "badFCS") {
                    data.badFCS = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "badFCSBytes") {
                    data.badFCSBytes = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "rTx") {
                    data.rTx = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "rTxBytes") {
                    data.rTxBytes = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "total") {
                    data.total = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "totalBytes") {
                    data.totalBytes = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "airTime") {
                    data.airTime = std::stoul(it.second.data());
                    num++;
                }
                else if (it.first == "bssId") {
                    // optional
                    data.bssId = senf::MACAddress::from_string(it.second.data());
                }
                else if (it.first == "ssId") {
                    // optional
                    data.ssId = it.second.data();
                }
                else if (it.first == "type") {
                    // optional
                    data.type = it.second.data();
                }
                else if (it.first == "lastSeen") {
                    // optional
                    data.lastSeen = senf::ClockService::milliseconds(std::stoul(it.second.data()));
                }
            }
            if (num == 8) {
                if (data.totalBytes > 0 and data.lastSeen <= maxAge) {
                    // only add entries with activity
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

static void fast_split(char *str, char sep, std::vector<char *> & tokens)
{
    unsigned int start = 0, stop;
    for (stop = 0; str[stop]; stop++) {
        if (str[stop] == sep) {
            str[stop] = '\0';
            tokens.emplace_back(str + start);
            start = stop + 1;
        } else if (str[stop] == '\n') {
            str[stop] = '\0';
        }
    }
    tokens.emplace_back(str + start);
}

static inline std::uint32_t atou(const char * str)
{
    std::uint32_t x (0);
    while (*str != '\0') {
        x = (x*10) + (*str - '0');
        ++str;
    }
    return x;
}

static inline std::uint64_t atoull(const char * str)
{
    return strtoull(str, NULL, 10);
}

prefix_ bool senf::emu::WifiStatistics::pollStatisticsCSV(std::uint32_t tag, senf::ClockService::clock_type const & maxAge)
{
    if (timestamp_ and (tag_ == tag))
        return true;
    
    map_.clear();
    timestamp_ = senf::ClockService::clock_type(0);

    try {
        FILE * statsFile;
        char  dataLine[512];
        if ((statsFile = fopen((debugFsPath_ + "stats_csv").c_str(), "r")) != NULL) {
            while (fgets(dataLine, sizeof(dataLine) -1, statsFile)) {
                std::vector<char *> tokens;
                fast_split(dataLine, ',', tokens);
                if (tokens.size() == 32) {
                    WifiStatisticsData data;
                    // Format: see wifi-statistics/station.c
                    data.signal         = StatisticAccumulator<std::int64_t>(atoi(tokens[1]), atoull(tokens[2]),
                                                                             atoi(tokens[3]), atoi(tokens[4]),
                                                                             atou(tokens[5])).data();
                    data.signalNonData  = StatisticAccumulator<std::int64_t>(atoi(tokens[6]), atoull(tokens[7]),
                                                                             atoi(tokens[8]), atoi(tokens[9]),
                                                                             atou(tokens[10])).data();
                    data.bitrate        = StatisticAccumulator<std::uint64_t>(atou(tokens[11]), atoull(tokens[12]),
                                                                             atou(tokens[13]), atou(tokens[14]),
                                                                             atou(tokens[15])).data();
                    data.bitrateNonData = StatisticAccumulator<std::uint64_t>(atou(tokens[16]), atoull(tokens[17]),
                                                                             atou(tokens[18]), atou(tokens[19]),
                                                                             atou(tokens[20])).data();

                    data.badFCS      = atou(tokens[21]);
                    data.badFCSBytes = atou(tokens[22]);
                    data.rTx         = atou(tokens[23]);
                    data.rTxBytes    = atou(tokens[24]);
                    data.total       = atou(tokens[25]);
                    data.totalBytes  = atou(tokens[26]);
                    data.airTime     = atou(tokens[27]);
                    data.lastSeen    = senf::ClockService::milliseconds(atou(tokens[28]));
                    data.bssId       = senf::MACAddress::from_string(tokens[29]);
                    data.type        = tokens[30];
                    data.ssId        = tokens[31];
                    if (data.totalBytes > 0 and data.lastSeen <= maxAge) {
                        // only add entries with activity
                        map_.emplace(std::make_pair(senf::MACAddress::from_string(tokens[0]), data));
                    }
                } else {
                    invalidEntries_++;
                }
            }
            fclose(statsFile);
        } else {
            ioErrors_++;
            map_.clear();
            return false;
        }
    } catch (...) {
        ioErrors_++;
        map_.clear();
        return false;
    };

    tag_ = tag;
    timestamp_ = senf::scheduler::now();

    return true;
}

prefix_ bool senf::emu::WifiStatistics::pollStatisticsBIN(std::uint32_t tag, senf::ClockService::clock_type const & maxAge)
{
    if (timestamp_ and (tag_ == tag))
        return true;
    
    map_.clear();
    timestamp_ = senf::ClockService::clock_type(0);

    try {
        mmapFile stats(debugFsPath_ + "stats_bin");
        while (!stats.eof()) {
            WifiStatisticsData data;
            senf::MACAddress mac (senf::MACAddress::from_data(stats.next(6)));
            // Format: see wifi-statistics kernel module
            data.signal         = StatisticAccumulator<std::int64_t>(stats.next<std::int32_t>(), stats.next<std::uint64_t>(),
                                                                     stats.next<std::int32_t>(), stats.next<std::int32_t>(),
                                                                     stats.next<std::uint32_t>()).data();
            data.signalNonData  = StatisticAccumulator<std::int64_t>(stats.next<std::int32_t>(), stats.next<std::uint64_t>(),
                                                                     stats.next<std::int32_t>(), stats.next<std::int32_t>(),
                                                                     stats.next<std::uint32_t>()).data();
            data.bitrate        = StatisticAccumulator<std::int64_t>(stats.next<std::uint32_t>(), stats.next<std::uint64_t>(),
                                                                     stats.next<std::uint32_t>(), stats.next<std::uint32_t>(),
                                                                     stats.next<std::uint32_t>()).data();
            data.bitrateNonData = StatisticAccumulator<std::int64_t>(stats.next<std::uint32_t>(), stats.next<std::uint64_t>(),
                                                                     stats.next<std::uint32_t>(), stats.next<std::uint32_t>(),
                                                                     stats.next<std::uint32_t>()).data();
            
            data.total       = stats.next<std::uint32_t>();
            data.totalBytes  = stats.next<std::uint32_t>();
            data.badFCS      = stats.next<std::uint32_t>();
            data.badFCSBytes = stats.next<std::uint32_t>();
            data.rTx         = stats.next<std::uint32_t>();
            data.rTxBytes    = stats.next<std::uint32_t>();
            data.airTime     = stats.next<std::uint32_t>();
            data.lastSeen    = senf::ClockService::milliseconds(stats.next<std::uint32_t>());
            data.bssId       = senf::MACAddress::from_data(stats.next(6));
            switch (stats.next<std::uint32_t>()) {
            case 0:
                data.type = "UNKNOWN";
                break;
            case 1:
                data.type = "AP";
                break;
            case 2:
                data.type = "STA";
                break;
            case 3:
                data.type = "ADHOC";
                break;
            case 4:
                data.type = "MESH";
                break;
            };
            data.ssId        = std::string((char*)stats.next(32+4));
            if (data.totalBytes > 0 and data.lastSeen <= maxAge) {
                // only add entries with activity
                map_.emplace(std::make_pair(mac, data));
            }
        }
    } catch (...) {
        ioErrors_++;
        map_.clear();
        return false;
    };

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
    pollStatisticsCSV(tag, maxAge);
    return map_;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
