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


/** \file
    \brief MonitorDataFilter header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_MonitorDataFilter_
#define HH_SENF_Ext_NetEmu_WLAN_MonitorDataFilter_ 1

// Custom includes
#include <boost/unordered/unordered_map.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <senf/PPI/Module.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>
#include <senf/Packets/80211Bundle/WLANPacket.hh>
#include "TSFTHistogram.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    const short DEFAULT_WLAN_NOISE=-110;

    // forward declarations:
    class WLANModulationParameterRegistry;

    SENF_LOG_DEFINE_NAMED_AREA( WlanLogArea, "emu::WLAN");


    struct MonitorDataFilterStatistics
    {
        senf::ClockService::clock_type tstamp;
        unsigned received;
        unsigned freqMismatch;
        unsigned legacy;
        unsigned ht;
        unsigned vht;
        unsigned badFCS;
        unsigned unknownMCS;
        unsigned unknownType;
        unsigned truncated;
        unsigned retries;
        unsigned duplicated;
        unsigned aggregated;
        unsigned management;
        unsigned control;
        unsigned data;
        unsigned encrypted;
        unsigned substitute;
        unsigned lost;
        unsigned maxReorderSize;
        unsigned reorderOverflows;
        unsigned reorderResync;
        unsigned reordered;
        unsigned reorderedTimedOut;
        unsigned seqNoExpired;

        MonitorDataFilterStatistics();
        senf::ClockService::clock_type duration() const{
            return senf::ClockService::now() - tstamp;
        }
        void reset();
        MonitorDataFilterStatistics stats();
        void dump(std::ostream & os) const;
        std::string dump() const;
    };


    class MonitorDataFilter
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(MonitorDataFilter);

    public:
        ppi::connector::PassiveInput<RadiotapPacket> input;
        ppi::connector::PassiveInput<EthernetPacket> input_plain;
        ppi::connector::ActiveOutput<EthernetPacket> output;

        MonitorDataFilter(senf::MACAddress const & id = senf::MACAddress::None);

        void promisc(bool p);
        void annotate(bool a);
        bool annotate() const;
        void airTime(bool a);
        void frequency(unsigned f);
        void id(senf::MACAddress const & _id);
        TSFTHistogram & tsftHistogram();
        MonitorDataFilterStatistics stats();

        void flushQueues();

        void dropUnknownMCS(bool q);
        void dumpState(std::ostream & os) const;

        template <typename BSDSocket>
        static void filterMonitorTxFrames(BSDSocket sock);
        template <typename BSDSocket>
        static void filterMonitorRxFrames(BSDSocket sock);
        template <typename BSDSocket>
        static void filterMonitorTxAndCtlFrames(BSDSocket sock);

    private:
        struct ReorderRecord
        {
            unsigned nextExpectedSeqNo;
            senf::ClockService::clock_type timeout;
            std::deque<senf::EthernetPacket> queue;
        };
        struct SequenceNumber
        {
            unsigned number;
            senf::ClockService::clock_type last;
            SequenceNumber( unsigned number_, senf::ClockService::clock_type const & last_) : number(number_), last(last_) {}
        };
        typedef boost::unordered_map<boost::uint64_t, SequenceNumber> SequenceNumberMap;
        typedef boost::unordered_map<boost::uint64_t, ReorderRecord> ReorderMap;

        senf::ClockService::clock_type maxReorderDelay_;
        senf::scheduler::TimerEvent reorderQueueTimer_;
        SequenceNumberMap sequenceNumberMap_;
        ReorderMap reorderMap_;

        senf::MACAddress id_;
        bool promisc_;
        bool annotate_;
        bool airTime_;
        unsigned frequency_;
        WLANModulationParameterRegistry const & modulationRegistry_;
        TSFTHistogram tsftHistogram_;
        MonitorDataFilterStatistics stats_;
        bool dropUnknownMCS_;
        std::uint32_t ampduRefNo_;

        void resetTimer();
        void reorderQueueTick();
        void handleReorderedPacket(SequenceNumberMap::key_type key, unsigned & lastSeqNo, unsigned seqNo, senf::EthernetPacket & ethp);
        void flushQueue(SequenceNumberMap::key_type key);

        void pushSubstituteEthernet(RadiotapPacket const & rtPacket);

        void outExtUI(Packet const & pkt, senf::MACAddress const & src = senf::MACAddress::None, senf::MACAddress const & dst = senf::MACAddress::Broadcast);
        void outData(senf::EthernetPacket const & eth);

        void handle_badFCS(RadiotapPacket const & rtp);
        void handle_DuplicateFrame(EthernetPacket const & eth);
        bool handle_ManagementFrame(RadiotapPacket const & rtPacket, WLANPacket_MgtFrame const & mgt);
        bool handle_CtrlFrame(RadiotapPacket const & rtPacket, WLANPacket_CtrlFrame const & ctrl);
        void handle_NonDataFrame(RadiotapPacket const & rtPacket);
        void handle_NonQoSData(RadiotapPacket const & rtPacket);

        void request();
        void requestPlain();
    };
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MonitorDataFilter.cci"
#include "MonitorDataFilter.ct"
//#include "MonitorDataFilter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
