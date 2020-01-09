// $Id:$
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

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
