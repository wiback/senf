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


#ifndef __CONFIGURATION_HH__
#define __CONFIGURATION_HH__

// Custom includes
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Utils/Logger.hh>

///////////////////////////////hh.p////////////////////////////////////////

class Configuration
{
    SENF_LOG_CLASS_AREA();

public:
    bool verbose;
    std::string phyName;
    std::string debugFS;
    senf::ClockService::clock_type reportingInterval;
    senf::ClockService::clock_type duration;
    unsigned frequency;
    bool ht40;

    unsigned spectralPeriod;
    unsigned spectralFFTPeriod;
    unsigned spectralCount;
    unsigned spectralBins;
    bool spectralEndless;
    bool spectralShortRepeat;
    
    Configuration();

    void help();
    void version();
    void enableHighresTimers();
    void enableRealtimeScheduling();

    bool parse(int argc, char const *argv[]);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif
