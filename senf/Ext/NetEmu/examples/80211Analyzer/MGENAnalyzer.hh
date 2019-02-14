// $Id:$
//
// Copyright (C) 2012
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

#ifndef __MGENANALYZER_HH__
#define __MGENANALYZER_HH__

// Custom includes
#include "unordered_map"
#include "FrameAnalyzer.hh"

///////////////////////////////hh.p////////////////////////////////////////

class MGENAnalyzer : public FrameAnalyzer
{
private:
    typedef std::unordered_map<std::uint32_t,FlowStatistics> FlowStatisticsMap;
    FlowStatisticsMap flowStatsMap;
    bool mplsDetected_;

    FlowStatistics & getFlowStats(std::uint32_t flowId);
    float sumMgenFlow(senf::ClockService::clock_type const & actualDuration);

    void reportCSV(FlowStatistics & flow, senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);
    void reportHuman(FlowStatistics & flow, senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);

public:
    MGENAnalyzer(Configuration const & configuration);

    bool v_handleUDPPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap, senf::IPv4Packet const & ip4, senf::UDPPacket const & udp);
    bool v_handleMPLSPacket(senf::EthernetPacket const & eth, senf::AnnotationsPacket const & ap, senf::MPLSPacket const & mpls, senf::TIMPacket const & tim);
    void v_report(senf::ClockService::clock_type const & timestamp, senf::ClockService::clock_type const & actualDuration);
};

///////////////////////////////hh.p////////////////////////////////////////
#endif


