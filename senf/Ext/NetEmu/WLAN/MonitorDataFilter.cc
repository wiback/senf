// $Id:$
//
// Copyright (C) 2009
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief MonitorDataFilter non-inline non-template implementation */

#include "MonitorDataFilter.hh"

// Custom includes
#include "senf/PPI/QueueSocketSourceSink.hh"
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>
#include <senf/Utils/algorithm.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/Log.hh>
#include "WLANInterface.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

const short DEFAULT_WLAN_NOISE=-110;

#define MAC_AND_TC_TO_KEY(mac,tc) ((mac.uint64() << 8) | unsigned(tc))
#define NO_SEQ_NO INT_MAX
#define REORDER_MAX 64

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::MonitorDataFilterStatistics

prefix_ senf::emu::MonitorDataFilterStatistics::MonitorDataFilterStatistics()
{
    reset();
}

prefix_ void senf::emu::MonitorDataFilterStatistics::reset()
{
    memset( this, 0, sizeof(*this));
    tstamp = senf::ClockService::now();
}

prefix_ senf::emu::MonitorDataFilterStatistics senf::emu::MonitorDataFilterStatistics::stats()
{
    MonitorDataFilterStatistics tmp (*this);
    reset();
    return tmp;
}

prefix_ void senf::emu::MonitorDataFilterStatistics::dump(std::ostream & os)
    const
{
    os << "(duration "    << senf::ClockService::in_milliseconds(duration()) << " ms"
       << ",rate "        << (received*1000) / (senf::ClockService::in_milliseconds(duration())+1) << " pps";// +1 to avoid DIV_BY_ZERO

    if ((legacy + ht + unknownMCS) != received ||
        (management + control + data + unknownType + badFCS + truncated) != received ||
        duplicated > retries ||
        (truncated + badFCS + unknownType) != substitute)
        os << ",stats bad";
    else
        os << ",stats good";

    os.setf(std::ios::fixed,std::ios::floatfield);
    os.precision(1);

    os << ";received "    << received
       << ";legacy "      << legacy      << " (" << float(legacy)      / float(received) * 100.0f << "%)"
       << ",ht "          << ht          << " (" << float(ht)          / float(received) * 100.0f << "%)"
       << ",unknownMCS "  << unknownMCS  << " (" << float(unknownMCS)  / float(received) * 100.0f << "%)"
       << ";management "  << management  << " (" << float(management)  / float(received) * 100.0f << "%)"
       << ",control "     << control     << " (" << float(control)     / float(received) * 100.0f << "%)"
       << ",data "        << data        << " (" << float(data)        / float(received) * 100.0f << "%)"
       << ",unknownType " << unknownType << " (" << float(unknownType) / float(received) * 100.0f << "%)"
       << ",badFCS "      << badFCS      << " (" << float(badFCS)      / float(received) * 100.0f << "%)"
       << ",truncated "   << truncated   << " (" << float(truncated)   / float(received) * 100.0f << "%)"
       << ";retries "     << retries     << " (" << float(retries)     / float(received) * 100.0f << "%)"
       << ";duplicated "  << duplicated  << " (" << float(duplicated)  / float(received) * 100.0f << "%)"
       << ";encrypted "   << encrypted   << " (" << float(encrypted)   / float(received) * 100.0f << "%)"
       << ";substitute "  << substitute  << " (" << float(substitute)  / float(received) * 100.0f << "%)"
       << ";lost "        << lost        << " (" << float(lost)        / float(received) * 100.0f << "%)"
       << ";maxReorderSize "     << maxReorderSize
       << ";reordered "          << reordered
       << ";reorderedTimedOut "  << reorderedTimedOut
       << ";reorderResync "      << reorderResync
       << ";reorderOverflows "   << reorderOverflows;

    os << ")" << std::endl;
}

prefix_ std::string senf::emu::MonitorDataFilterStatistics::dump()
    const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::MonitorDataFilter

prefix_ senf::emu::MonitorDataFilter::MonitorDataFilter(senf::MACAddress const & id)
    : maxReorderDelay_ (senf::ClockService::milliseconds(32)),
      reorderQueueTimer_( "ReorderQueueTimer_" + senf::str(id), senf::membind( &MonitorDataFilter::reorderQueueTick, this)),
      id_(id), promisc_(false),
      modulationRegistry_(WLANModulationParameterRegistry::instance()),
      dropUnknownMCS_ (true),
      detailedAnnotations_(false)
{
    noroute(monitor);
    route(input, output).autoThrottling(false);
    input.onRequest( &MonitorDataFilter::request);
    input.throttlingDisc( ppi::ThrottlingDiscipline::NONE);
}

prefix_ void senf::emu::MonitorDataFilter::dropUnknownMCS(bool q)
{
    dropUnknownMCS_ = q;
}

prefix_ void senf::emu::MonitorDataFilter::promisc(bool p)
{
    promisc_ = p;
}

prefix_ senf::emu::TSFTHistogram & senf::emu::MonitorDataFilter::tsftHistogram()
{
    return tsftHistogram_;
}

prefix_ void senf::emu::MonitorDataFilter::id(senf::MACAddress const & _id)
{
    id_ = _id;
}

prefix_ senf::emu::MonitorDataFilterStatistics senf::emu::MonitorDataFilter::stats()
{
    return stats_.stats();
}

prefix_ void senf::emu::MonitorDataFilter::handle_DuplicateFrame(EthernetPacket & eth)
{
    stats_.duplicated++;
    if (promisc_) {
        {
            eth.annotation<annotations::Quality>().flags.frameDuplicate = true;
        }
        output(eth);
    }
}

static inline int seqNoDelta(unsigned last, boost::uint16_t current)
{
    if (last == NO_SEQ_NO) return INT_MAX;
    // modulo subtraction with 12-bit sign extension
    // see https://graphics.stanford.edu/~seander/bithacks.html
    return (((current - last) & 0x0FFF) ^ 0x0800) - 0x0800;
}

prefix_ void senf::emu::MonitorDataFilter::resetTimer()
{
    if (reorderMap_.empty()) {
        reorderQueueTimer_.disable();
        return;
    }

    senf::ClockService::clock_type timeout (reorderMap_.begin()->second.timeout);
    
    for (auto const i : reorderMap_) {
        if (i.second.timeout < timeout)
            timeout = i.second.timeout;
    }

    reorderQueueTimer_.timeout(timeout);
    reorderQueueTimer_.enable();
}

prefix_ void senf::emu::MonitorDataFilter::reorderQueueTick()
{
    for (ReorderMap::iterator i (reorderMap_.begin()); i != reorderMap_.end();) {
        if (i->second.timeout <= reorderQueueTimer_.timeout()) {
            ++ stats_.reorderedTimedOut;
            for (senf::EthernetPacket const & ethp : i->second.queue) {
                if (ethp)
                    output(ethp);
            }
            sequenceNumberMap_.erase(i->first);
            i = reorderMap_.erase(i);
        }
        else
            ++ i;
    }

    resetTimer();
}

prefix_ void senf::emu::MonitorDataFilter::flushQueues()
{
    reorderMap_.clear();
    resetTimer();
    sequenceNumberMap_.clear();
    stats_.reset();
}

prefix_ void senf::emu::MonitorDataFilter::flushQueue(SequenceNumberMap::key_type key)
{
    ReorderMap::iterator i (reorderMap_.find(key));
    if (i != reorderMap_.end()) {
        while (!i->second.queue.empty()) {
            if (i->second.queue.front())
                output(i->second.queue.front());
            i->second.queue.pop_front();
        }
    }
    reorderMap_.erase(key);
    resetTimer();
    sequenceNumberMap_.erase(key);
}

prefix_ void senf::emu::MonitorDataFilter::handleReorderedPacket(SequenceNumberMap::key_type key, unsigned & lastSeqNo,
                                                                 unsigned seqNo, senf::EthernetPacket & ethp)
{
    ++ stats_.reordered;
    ReorderMap::iterator i (reorderMap_.find(key));
    
    SENF_ASSERT( (lastSeqNo != NO_SEQ_NO) == (i == reorderMap_.end()),
                 "reorderMap / sequenceNumberMap inconsistency" );

    if (i == reorderMap_.end()) {
        // We know that seqNo must be at least 2 packets ahead of lastSeqNo, otherwise
        // handleReorderPacket would not have been called.
        i = reorderMap_.insert(std::make_pair(key, ReorderRecord())).first;
        resetTimer();
        i->second.nextExpectedSeqNo = (lastSeqNo + 1) & 0x0FFF;
        i->second.queue.push_back(senf::EthernetPacket());
        lastSeqNo = NO_SEQ_NO;
    }
    {
        ReorderRecord & record (i->second);
        
        int delta (seqNoDelta(record.nextExpectedSeqNo, seqNo));
        if (delta <= REORDER_MAX) {
            if (delta < 0) {
                handle_DuplicateFrame(ethp);
                return;
            } else if (unsigned(delta) >= record.queue.size()) {
                record.queue.resize(delta + 1);
                if (record.queue.size() > stats_.maxReorderSize)
                    stats_.maxReorderSize = record.queue.size();
            }
            else if (record.queue.at(delta)) {
                handle_DuplicateFrame(ethp);
                return;
            }
            ethp.data().releaseExternalMemory();
            record.queue.at(delta) = ethp;
            
            while (! record.queue.empty() && record.queue.front()) {
                seqNo = record.nextExpectedSeqNo;
                output(record.queue.front());
                record.queue.pop_front();
                record.nextExpectedSeqNo = (record.nextExpectedSeqNo + 1) & 0x0FFF;
            }
        } else {
            ++ stats_.reorderOverflows;
            unsigned gap (0);  // we determine the SeqNo gaps and report them via the annotation
            while (! record.queue.empty()) {
                if (record.queue.front()) {
                    {
                        record.queue.front().annotation<annotations::Quality>().setLoss(gap);
                        stats_.lost += gap;
                    }
                    output(record.queue.front());
                    gap = 0;
                } else {
                    gap++;
                }
                record.queue.pop_front();
            }
            // ethp is the first packet which would have increased the queue size beyond
            // REORDER_MAX. Thus the sequence number of ethp (seqNo) must be beyond all other
            // sequence numbers in the queue and thus this is the last packet.
            output(ethp);
            // We don't have to explicitly set seqNo since it's already correct.
        }
        
        if (record.queue.empty()) {
            // invalidates 'record' variable !!
            reorderMap_.erase(i);
            lastSeqNo = seqNo;
        }
        else
            record.timeout = senf::ClockService::now() + maxReorderDelay_;
    }

    resetTimer();
}

prefix_ void senf::emu::MonitorDataFilter::pushSubstituteEthernet(RadiotapPacket & rtp)
{
    // only push fake frame if promisc mode is on
    if (!promisc_)
        return;

    MACAddress src (senf::MACAddress::None);
    try {
        WLANPacket_DataFrame data (rtp.next<WLANPacket_DataFrame>(senf::nothrow));
        if (data && (data.size() >= WLANPacket_DataFrameParser::init_bytes)) {
            src = data->sourceAddress();
        } else {
            WLANPacket_MgtFrame mgt (rtp.next<WLANPacket_MgtFrame>(senf::nothrow));
            if (mgt && (mgt.size() >= WLANPacket_MgtFrameParser::init_bytes)) {
                src = mgt->sourceAddress();
            } else {
                WLANPacket_CtrlFrame ctrl (rtp.next<WLANPacket_CtrlFrame>(senf::nothrow));
                if (ctrl && ctrl->is_rts() && (ctrl.size() >= WLANPacket_CtrlFrameParser::init_bytes)) {
                    src = ctrl->sourceAddress();
                }
            }
        }
    }
    catch (...) {
        // catch any possible exception here.
        // If we catch one, we assume that we failed to extract a valid/usefull source address
        src = senf::MACAddress::None;
    }

    // Check if we know this SRC so that we can assume it to be valid....
    if (src != senf::MACAddress::None) {
        unsigned qos;
        for (qos = 0; qos < 8; qos++) {
            if (sequenceNumberMap_.find( MAC_AND_TC_TO_KEY(src, qos)) != sequenceNumberMap_.end()) {
                break;
            }
        }
        
        if (qos == 8) {
            // ...else, we use None
            src = MACAddress::None;
        }
    }

    // build a fake ethernet frame which has the original annotations and the original size
    // the source is either the actual source, or None (any)
    // and the destination is always broadcast
    // Hence, this frame mimics a frame sent from someone to all within its cell
    // The main purpose is that i.e. channel scanners can properly capture the activity on a channel
    senf::Packet pkt;
    try {
        pkt = rtp.next(senf::nothrow);
    }
    catch (...) {
        // no next() packet after radiotap !?!
        return;
    }

    if (!pkt)
        return;

    EthernetPacket eth (EthernetPacket::create( pkt.size() < EthernetPacketParser::fixed_bytes ? EthernetPacketParser::fixed_bytes : pkt.size()));
    eth->source() = src;
    eth->destination() = MACAddress::Broadcast;
    eth->type_length() = 0xffff; // reserved

    eth.annotation<annotations::Quality>()               = rtp.annotation<annotations::Quality>();
    eth.annotation<annotations::Interface>().value       = rtp.annotation<annotations::Interface>().value;
    eth.annotation<annotations::Timestamp>()             = rtp.annotation<annotations::Timestamp>();
    eth.annotation<annotations::WirelessModulation>().id = rtp.annotation<annotations::WirelessModulation>().id;

    stats_.substitute++;
    output( eth);
}


prefix_ void senf::emu::MonitorDataFilter::handle_badFCS(RadiotapPacket & rtPacket)
{
    stats_.badFCS++;
    rtPacket.annotation<annotations::Quality>().flags.frameCorrupt = true;
    pushSubstituteEthernet( rtPacket);
}

prefix_ bool senf::emu::MonitorDataFilter::handle_ManagementFrame(RadiotapPacket & rtPacket, WLANPacket_MgtFrame & mgt)
{
    if (mgt.size() < WLANPacket_MgtFrameParser::init_bytes) {
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping truncated management frame") );
        stats_.truncated++;
        return false;
    }

    /*
    // (DE)AUTH Frame
    if (mgt->subtype() == 11 && mgt->destinationAddress() == id_) {
        for (boost::uint16_t tc (0); tc < 65535; tc++)
        sequenceNumberMap_.erase( MAC_AND_TC_TO_KEY( mgt->sourceAddress(), tc));
    }
    */

    stats_.management++;
    if (monitor.connected())
        monitor(rtPacket);

    return true;
}

prefix_ bool senf::emu::MonitorDataFilter::handle_CtrlFrame(RadiotapPacket & rtPacket, WLANPacket_CtrlFrame & ctrl)
{
    if (ctrl.size() < WLANPacket_CtrlFrameParser::init_bytes) {
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping truncated ctrl frame") );
        stats_.truncated++;
        return false;
    }

    stats_.control++;
    if (monitor.connected())
        monitor(rtPacket);

    return true;
}


prefix_ void senf::emu::MonitorDataFilter::handle_NonDataFrame(RadiotapPacket & rtPacket)
{
    // 
    // This might be somewhat less efficient, but we should just see a few frames per second (as we filter ACKs via BPF) 
    //
    WLANPacket_MgtFrame mgt (rtPacket.next<WLANPacket_MgtFrame>(senf::nothrow));
    if (mgt) {
        if (!handle_ManagementFrame( rtPacket, mgt)) {
            pushSubstituteEthernet( rtPacket);
        }
        return;
    }
    WLANPacket_CtrlFrame ctrl (rtPacket.next<WLANPacket_CtrlFrame>(senf::nothrow));
    if (ctrl) {
        if (!handle_CtrlFrame( rtPacket, ctrl)) {
            pushSubstituteEthernet( rtPacket);
        }
        return;
    }

    // If we make it here, the frame type is not known (most likely, it is encrypted!)
    try {
        DataPacket data (rtPacket.parseNextAs<DataPacket>());
        WLANPacketParser wlanParser (data.data().begin(), &data.data());
        stats_.unknownType++;
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping frame of unknown frame type: "
                << unsigned(wlanParser.type()) << "," << unsigned(wlanParser.subtype()) ));
    } catch (senf::TruncatedPacketException &) {
        stats_.truncated++;
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") unknown frame is truncated") );
    } catch (senf::InvalidPacketChainException &) {
        stats_.truncated++;
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") radiotap frame is truncated") );
    }

    pushSubstituteEthernet( rtPacket);
}

prefix_ void senf::emu::MonitorDataFilter::request()
{
    try {
        RadiotapPacket rtPacket (input());
        RadiotapPacket::Parser rtParser (rtPacket.parser());
        
        stats_.received++;

        // Set Annotations: incoming interface
        rtPacket.annotation<annotations::Interface>().value = id_;

        // set the rx timestamp. Careful: this assumes that we are using an MMAP source !
        rtPacket.annotation<annotations::Timestamp>().fromQueueBuffer(*(rtPacket.annotation<senf::ppi::QueueBufferAnnotation>().value));

        // Set annotations: Quality
        {
            annotations::Quality & q (rtPacket.annotation<annotations::Quality>());
            q.rssi  = std::min( short(0), short(rtParser.dbmAntennaSignal()));
//            q.noise = (rtParser.dbmAntennaNoisePresent() ? short(rtParser.dbmAntennaNoise()) : short(DEFAULT_WLAN_NOISE));
            q.noise = short(DEFAULT_WLAN_NOISE);
            q.snr   = std::max( short(0), (short(q.rssi - q.noise)));
            q.flags.frameLength = rtPacket.size() - rtParser.length();
        }

        /*
        // determine MCS (rate)
        if (rtParser.mcsPresent()) {
            stats_.ht++;
            rtPacket.annotation<annotations::WirelessModulation>().id = modulationRegistry_.parameterIdByMCS(
            rtParser.mcs().mcsIndex(),
                    rtParser.mcs().bandwidth(),
                    rtParser.mcs().guardInterval() );
            }
            else if (rtParser.ratePresent()) {
                stats_.legacy++;
                rtPacket.annotation<annotations::WirelessModulation>().id = modulationRegistry_.parameterIdByLegacyRate( rtParser.rate() * 500);
            }
            else {
                stats_.unknownMCS++;
                if (dropUnknownMCS_) {
                    SENF_LOG( (WlanLogArea) (senf::log::IMPORTANT)
                              ( "(" << id_ << ") dropping packet with unknown datarate "));
                    return;
                }
            }
        }
        */

        // Catch frames with bad FCS...
        if (rtParser.flags().badFCS()) {
            handle_badFCS(rtPacket);
            return;
        }
        
        unsigned radiotapLength (senf::bytes(rtParser));
        senf::WLANPacket_DataFrameParser wlan (rtPacket.data().begin() + radiotapLength, &rtPacket.data());
        if (wlan.type() != 2 || wlan.subtype() != 8) {
            handle_NonDataFrame(rtPacket);
            return;
        }

        senf::MACAddress src (wlan.sourceAddress());
        senf::MACAddress dst (wlan.destinationAddress());

        /*
        // TSFT Histogram
        if (tsftHistogram_.enabled()) {
            tsftHistogram_.update( src, rtParser.tsft());
        }
        */

        if (dst != id_ && !dst.multicast() && !promisc_)
            return;

        // We have 16 unused bits in the 'key' since the MAC address is only 48 bits.
        // So let's use some of them to indicate the TID or the default (no QoS Data) seqNo sequence (we use 0xff for that)
        // If we have a QoSData frame, we need to keep separate seq# counters per TID (bits 0...3) for UNICAST traffic
        // If we have a regular Data frame, we use the same queue as for bcast and management frames
        unsigned seqNo (wlan.sequenceNumber());
        SequenceNumberMap::key_type key (MAC_AND_TC_TO_KEY(src, wlan.qosField()));

        // flag retransmission
        {
            stats_.retries += rtPacket.annotation<annotations::Quality>().flags.frameRetransmitted = wlan.retry();
        }

        senf::EthernetPacket eth (
            rtPacket.replaceAs<senf::EthernetPacket>(
                radiotapLength + senf::bytes(wlan) + senf::LlcSnapPacketParser::fixed_bytes
                - senf::EthernetPacketParser::fixed_bytes, -4));
        
        eth->source() << src;
        eth->destination() << dst;
        
        SequenceNumberMap::iterator i (sequenceNumberMap_.find(key));
        if (i != sequenceNumberMap_.end()) {
            int delta (seqNoDelta(i->second, seqNo));
            if (delta == 1) {
                i->second = seqNo;
            } 
            else if (delta < -REORDER_MAX) {
                stats_.reorderResync++;
                flushQueue(key);
                sequenceNumberMap_.insert(std::make_pair(key, seqNo));
            } 
            else if (delta > 1) {
                handleReorderedPacket(key, i->second, seqNo, eth);
                return;
            } 
            else {
                handle_DuplicateFrame(eth);
                return;
            }
        } else {
            sequenceNumberMap_.insert(std::make_pair(key, seqNo));
        }

        stats_.data++;
        output(eth);
    }
    catch (senf::TruncatedPacketException const &) {
       stats_.truncated++;
    }
}

prefix_ void senf::emu::MonitorDataFilter::dumpState(std::ostream & os)
    const
{
    os << "reorderMap.size() = " << reorderMap_.size() << ", reorderQueueTimer is " << senf::str(reorderQueueTimer_.enabled()) << std::endl;
    for (auto const & seqNo : sequenceNumberMap_) {
        ReorderMap::const_iterator i (reorderMap_.find(seqNo.first));
        os << std::hex << "0x" << seqNo.first << std::dec << " => " << seqNo.second << ", queueSize: " << (i==reorderMap_.end() ? "(none)" : senf::str(i->second.queue.size())) << std::endl;
    } 
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
