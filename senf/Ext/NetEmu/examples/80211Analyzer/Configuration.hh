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
    bool mgenMode;
    bool iperfMode;
    bool bindSocket;
    bool tsftHistogram;
    bool verbose;
    bool csvMode;
    bool reorderPackets;
    bool promisc;
    float clockDrift;

    senf::ClockService::clock_type reorderTimeout;
    senf::ClockService::clock_type reportingInterval;
    senf::ClockService::clock_type duration;
    senf::ClockService::clock_type maxWait;
    std::string device;
    bool monitorMode;
    bool analyzeTx;
    senf::INet4SocketAddress source;
    senf::INet4SocketAddress destination;
    senf::INet4SocketAddress logDestination;
    std::uint64_t numPackets;

    Configuration();

    void help();
    void version();
    void enableHighresTimers();
    void enableRealtimeScheduling();

    void dev(std::string const & d);
    void monDev(std::string const & d);

    bool parse(int argc, char const *argv[]);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif
