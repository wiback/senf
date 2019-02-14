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

#include "PacketStatistics.hh"

// Custom includes
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/WLAN/WLANInterface.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ PacketStatistics::PacketStatistics()
{
    v_reset();
}

prefix_ void PacketStatistics::v_reset()
{
    count = 0;
    bytes = 0;
    retry = 0;
    airtime = senf::ClockService::seconds(0);
}

prefix_ bool PacketStatistics::analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize)
{
    unsigned rateInBps (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate);

    if (ap->duplicated()) {
        // we've already seen this frame, so just count it as a 'retry'
        retry++;
        return false;
    }

    count++;
    bytes += payloadSize;
    if (ap->retransmitted())
        retry++;

    airtime += senf::ClockService::microseconds( (ap->length() * 8 * 1000000) / rateInBps);

    return true;
}

prefix_ bool PacketStatistics::v_analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize)
{
    if (payloadSize == 0)
        payloadSize = ap->length();

    return analyze(pkt, ap, payloadSize);
}

prefix_ float PacketStatistics::pktsPerSecond(senf::ClockService::clock_type reportingInterval)
{
    return float(count * 1000000) / float(senf::ClockService::in_microseconds( reportingInterval));
}

prefix_ float PacketStatistics::bitsPerSecond(senf::ClockService::clock_type reportingInterval)
{
    return float(bytes * 8 * 1000000) / float(senf::ClockService::in_microseconds( reportingInterval));
}

prefix_ float PacketStatistics::retryPerSecond(senf::ClockService::clock_type reportingInterval)
{
    return float((retry * 1000000)) / float(senf::ClockService::in_microseconds( reportingInterval));
}

prefix_ senf::ClockService::clock_type PacketStatistics::airtimePerSecond(senf::ClockService::clock_type reportingInterval)
{
    return airtime / reportingInterval;
}

prefix_ FlowStatistics::FlowStatistics()
    : loss( 4294967295u)  // 32bit maximum
{
}

prefix_ void FlowStatistics::v_reset()
{
    PacketStatistics::v_reset();

    rssi.clear();
    noise.clear();
    rate.clear();
    length.clear();
    latency.clear();
    loss.reset();
}

prefix_ bool FlowStatistics::v_analyze(senf::Packet const & pkt, senf::AnnotationsPacket const & ap, unsigned payloadSize)
{
    unsigned rateInBps (senf::emu::WLANModulationParameterRegistry::instance().findModulationById(ap->modulationId()).rate);

    if (!PacketStatistics::analyze(pkt, ap, payloadSize))
        return false;

    if (ap->rssi() != 0) {
        rssi.accumulate(ap->rssi());
    }
    //noise.accumulate( ap->noise());
    rate.accumulate( rateInBps);
    length.accumulate( payloadSize);

    return true;
}

prefix_ bool FlowStatistics::v_analyze(MGENPacket const & mgen, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime)
{
    if (!v_analyze(mgen, ap, payloadSize))
        return false;

    // calculate the packet latency: clocks must be synced !!!
    timeval tv;
    gettimeofday( &tv, NULL);
    std::int64_t current = std::int64_t(tv.tv_sec)             * 1000000 + std::int64_t(tv.tv_usec);
    std::int64_t packet  = std::int64_t(mgen->txTimeSeconds()) * 1000000 + std::int64_t(mgen->txTimeMicroseconds());
    float clockDriftOffset = (float(senf::ClockService::in_microseconds(senf::ClockService::now() - startTime)) * clockDrift) / 1000000.0f;
    latency.accumulate( (current - packet) - std::int64_t(clockDriftOffset));

    // now, calculate the loss
    loss.update( mgen->sequenceNumber());

    return true;
}

prefix_ bool FlowStatistics::v_analyze(IperfUDPPacket const & iperf, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime)
{
    if (!v_analyze(iperf, ap, payloadSize))
        return false;

    loss.update( iperf->id());

    return true;
}

prefix_ bool FlowStatistics::v_analyze(senf::TIMPacket const & tim, senf::AnnotationsPacket const & ap, unsigned payloadSize, float clockDrift, senf::ClockService::clock_type startTime)
{
    if (!v_analyze(tim, ap, payloadSize))
        return false;

    // calculate the packet latency: clocks must be synced !!!
    timeval tv;
    gettimeofday( &tv, NULL);
    std::int64_t current = std::int64_t(tv.tv_sec)             * 1000000 + std::int64_t(tv.tv_usec);
    std::int64_t packet  = std::int64_t(tim->timestamp()) * 1000;
    float clockDriftOffset = (float(senf::ClockService::in_microseconds(senf::ClockService::now() - startTime)) * clockDrift) / 1000000.0f;
    latency.accumulate( (current - packet) - std::int64_t(clockDriftOffset));

    // now, calculate the loss
    loss.update(tim->sequenceNumber());

    return true;
}


prefix_ void FlowStatistics::getRssi(senf::StatisticsData & data)
{
    rssi.data( data);
}

prefix_ void FlowStatistics::getNoise(senf::StatisticsData & data)
{
    noise.data( data);
}

prefix_ void FlowStatistics::getRate(senf::StatisticsData & data)
{
    rate.data( data);
}

prefix_ void FlowStatistics::getLength(senf::StatisticsData & data)
{
    length.data( data);
}

prefix_ void FlowStatistics::getLatency(senf::StatisticsData & data)
{
    latency.data( data);
}

prefix_ float FlowStatistics::getLoss()
{
    return loss.getLoss();
}

prefix_ std::int64_t FlowStatistics::getPktsDuplicate()
{
    return loss.getPktsDuplicate();
}

prefix_ std::int64_t FlowStatistics::getPktsLate()
{
    return loss.getPktsLate();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
