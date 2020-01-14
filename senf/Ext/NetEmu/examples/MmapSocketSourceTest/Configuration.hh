//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef __CONFIGURATION_HH__
#define __CONFIGURATION_HH__

// Definition of non-inline non-template functions

// Custom includes
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Utils/Console.hh>


class Configuration
{
    SENF_LOG_CLASS_AREA();

public:
    bool verbose;
    boost::uint32_t numBuffers;
    std::string interface;
    std::string monitorInterface;
    std::string outputInterface;
    bool logSeqNoErrors;

    Configuration();

    void help();
    void version();
    void enableHighresTimers();
    void enableRealtimeScheduling();

    bool parse( int argc, char const *argv[]);
};

#endif
