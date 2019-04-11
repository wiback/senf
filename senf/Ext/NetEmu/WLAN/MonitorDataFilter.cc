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
#include <senf/Ext/NetEmu/Log.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>
#include "WLANInterface.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define NO_SEQ_NO INT_MAX
#define REORDER_MAX 192  // should be more than enough
#define SEQ_EXPIRE   32  // in ms

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

    if ((freqMismatch + legacy + ht + vht + unknownMCS) != received ||
        (freqMismatch + management + control + data + unknownType + badFCS + truncated) != received ||
        duplicated > retries ||
        (truncated + badFCS + unknownType) != substitute)
        os << ",stats bad";
    else
        os << ",stats good";

    os.setf(std::ios::fixed,std::ios::floatfield);
    os.precision(1);

    os << ";received "     << received
       << ";freqMismatch " << freqMismatch << " (" << float(freqMismatch) / float(received) * 100.0f << "%)"
       << ",legacy "       << legacy       << " (" << float(legacy)       / float(received) * 100.0f << "%)"
       << ",ht "           << ht           << " (" << float(ht)           / float(received) * 100.0f << "%)"
       << ",vht "          << vht          << " (" << float(vht)          / float(received) * 100.0f << "%)"
       << ",unknownMCS "   << unknownMCS   << " (" << float(unknownMCS)   / float(received) * 100.0f << "%)"
       << ";management "   << management   << " (" << float(management)   / float(received) * 100.0f << "%)"
       << ",control "      << control      << " (" << float(control)      / float(received) * 100.0f << "%)"
       << ",data "         << data         << " (" << float(data)         / float(received) * 100.0f << "%)"
       << ",unknownType "  << unknownType  << " (" << float(unknownType)  / float(received) * 100.0f << "%)"
       << ",badFCS "       << badFCS       << " (" << float(badFCS)       / float(received) * 100.0f << "%)"
       << ",truncated "    << truncated    << " (" << float(truncated)    / float(received) * 100.0f << "%)"
       << ";retries "      << retries      << " (" << float(retries)      / float(received) * 100.0f << "%)"
       << ";duplicated "   << duplicated   << " (" << float(duplicated)   / float(received) * 100.0f << "%)"
       << ";aggregated "   << aggregated   << " (" << float(aggregated)   / float(received) * 100.0f << "%)"
       << ";encrypted "    << encrypted    << " (" << float(encrypted)    / float(received) * 100.0f << "%)"
       << ";substitute "   << substitute   << " (" << float(substitute)   / float(received) * 100.0f << "%)"
       << ";lost "         << lost         << " (" << float(lost)         / float(received) * 100.0f << "%)"
       << ";reordered "          << reordered
       << ";maxReorderUsage "     << maxReorderSize << "/" << REORDER_MAX
       << ";reorderResync "      << reorderResync
       << ";reorderedTimedOut "  << reorderedTimedOut << " (timeout " << SEQ_EXPIRE << "ms)"
       << ";reorderOverflows "   << reorderOverflows
       << ";seqNoExpired "       << seqNoExpired;

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
    : maxReorderDelay_ (senf::ClockService::milliseconds(SEQ_EXPIRE)),
      reorderQueueTimer_( "ReorderQueueTimer_" + senf::str(id), senf::membind( &MonitorDataFilter::reorderQueueTick, this)),
      id_(id),
      promisc_(false),
      annotate_(false),
      airTime_(false),
      frequency_(0),
      modulationRegistry_(WLANModulationParameterRegistry::instance()),
      dropUnknownMCS_ (true),
      ampduRefNo_(0)
{
    route(input, output).autoThrottling(false);
    input.onRequest( &MonitorDataFilter::request);
    input.throttlingDisc( ppi::ThrottlingDiscipline::NONE);
    route(input_plain, output).autoThrottling(false);
    input_plain.onRequest( &MonitorDataFilter::requestPlain);
    input_plain.throttlingDisc( ppi::ThrottlingDiscipline::NONE);
}

prefix_ void senf::emu::MonitorDataFilter::dropUnknownMCS(bool q)
{
    dropUnknownMCS_ = q;
}

prefix_ void senf::emu::MonitorDataFilter::promisc(bool p)
{
    promisc_ = p;
}

prefix_ void senf::emu::MonitorDataFilter::airTime(bool a)
{
    airTime_ = a;
}

prefix_ void senf::emu::MonitorDataFilter::annotate(bool a)
{
    annotate_ = a;
}

prefix_ bool senf::emu::MonitorDataFilter::annotate()
    const
{
    return annotate_;
}

prefix_ void senf::emu::MonitorDataFilter::frequency(unsigned f)
{
    frequency_ = f;
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

prefix_ void senf::emu::MonitorDataFilter::handle_DuplicateFrame(EthernetPacket const & eth)
{
    stats_.duplicated++;
    if (promisc_) {
        eth.annotation<annotations::Quality>().flags.frameDuplicate = true;
        outData(eth);
    }
}

static inline std::int32_t seqNoDelta(std::uint16_t current, std::uint32_t last)
{
    if (SENF_UNLIKELY(last == NO_SEQ_NO)) return INT_MAX;

    static constexpr std::int32_t maxSeq_     = 0xfff+1;
    static constexpr std::int32_t threshold_  = maxSeq_/10;    

    std::int32_t dist(current - last);

    if (SENF_UNLIKELY(dist + threshold_ < 0))
        return dist + maxSeq_;
    if (SENF_UNLIKELY(dist - (maxSeq_ - threshold_) > 0))
        return dist - maxSeq_;
    
    return dist;

    // modulo subtraction with 12-bit sign extension
    // see https://graphics.stanford.edu/~seander/bithacks.html
    //return (((current - last) & 0x0FFF) ^ 0x0800) - 0x0800;
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

    timeout = std::max(timeout, senf::ClockService::now() + maxReorderDelay_ / 8);

    reorderQueueTimer_.timeout(timeout);
    reorderQueueTimer_.enable();
}

prefix_ void senf::emu::MonitorDataFilter::reorderQueueTick()
{
    for (ReorderMap::iterator i (reorderMap_.begin()); i != reorderMap_.end();) {
        if (i->second.timeout <= reorderQueueTimer_.timeout()) {
            ++ stats_.reorderedTimedOut;
            for (senf::EthernetPacket & ethp : i->second.queue) {
                if (ethp)
                    outData(ethp);
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
}

prefix_ void senf::emu::MonitorDataFilter::flushQueue(SequenceNumberMap::key_type key)
{
    ReorderMap::iterator i (reorderMap_.find(key));
    if (i != reorderMap_.end()) {
        while (!i->second.queue.empty()) {
            if (i->second.queue.front())
                outData(i->second.queue.front());
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
        i = reorderMap_.emplace(std::make_pair(key, ReorderRecord())).first;
        resetTimer();
        i->second.nextExpectedSeqNo = (lastSeqNo + 1) & 0x0FFF;
        i->second.queue.emplace_back(senf::EthernetPacket());
        lastSeqNo = NO_SEQ_NO;
    }
    {
        ReorderRecord & record (i->second);
        
        int delta (seqNoDelta(seqNo, record.nextExpectedSeqNo));
        if (delta < REORDER_MAX) {
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
                outData(record.queue.front());
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
                    outData(record.queue.front());
                    gap = 0;
                } else {
                    gap++;
                }
                record.queue.pop_front();
            }
            // ethp is the first packet which would have increased the queue size beyond
            // REORDER_MAX. Thus the sequence number of ethp (seqNo) must be beyond all other
            // sequence numbers in the queue and thus this is the last packet.
            outData(ethp);
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

prefix_ void senf::emu::MonitorDataFilter::pushSubstituteEthernet(RadiotapPacket const & rtp)
{
    // only push fake frame if promisc mode is on
    if (!promisc_)
        return;

    MACAddress src (senf::MACAddress::None);
    MACAddress dst (senf::MACAddress::Broadcast);
    try {
        SequenceNumberMap::key_type key;

        WLANPacket_DataFrame const & data (rtp.next<WLANPacket_DataFrame>(senf::nothrow));
        if (data && (data.size() >= WLANPacket_DataFrameParser::init_bytes)) {
            data->sourceAddress().hash(&key);
            src = data->sourceAddress();
        } else {
            WLANPacket_MgtFrame const & mgt (rtp.next<WLANPacket_MgtFrame>(senf::nothrow));
            if (mgt && (mgt.size() >= WLANPacket_MgtFrameParser::init_bytes)) {
                mgt->sourceAddress().hash(&key);
                src = mgt->sourceAddress();
            } else {
                WLANPacket_CtrlFrame const & ctrl (rtp.next<WLANPacket_CtrlFrame>(senf::nothrow));
                if (ctrl && ctrl->is_rts() && (ctrl.size() >= WLANPacket_CtrlFrameParser::init_bytes)) {
                    ctrl->sourceAddress().hash(&key);
                    src = ctrl->sourceAddress();
                }
            }
        }
        
        // Check if we know this SRC so that we can assume it to be valid....
        if (src) {
            boost::uint16_t qos;
            for (qos = 0; qos < 8; qos++) {
                *(((std::uint16_t *)&key)+3) = qos;  // see EthernetPacket.cci
                if (sequenceNumberMap_.find(key) != sequenceNumberMap_.end()) {
                    break;
                }
            }
            if (qos == 8) 
                src = MACAddress::None;
        }
    }
    catch (...) {
        // catch any possible exception here.
        // If we catch one, we assume that we failed to extract a valid/useful source address
        src = senf::MACAddress::None;
    }
    
    try {
        WLANPacket_DataFrame const & data (rtp.next<WLANPacket_DataFrame>(senf::nothrow));
        if (data && (data.size() >= WLANPacket_DataFrameParser::init_bytes)) {
            dst = data->destinationAddress();
        } else {
            WLANPacket_MgtFrame const & mgt (rtp.next<WLANPacket_MgtFrame>(senf::nothrow));
            if (mgt && (mgt.size() >= WLANPacket_MgtFrameParser::init_bytes)) {
                dst = mgt->destinationAddress();
            }
        }

        if( dst != id_)
            dst = senf::MACAddress::Broadcast;
    }
    catch (...) {
        // catch any possible exception here.
        // If we catch one, we assume that we failed to extract a valid/useful destination address
        dst = senf::MACAddress::Broadcast;
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

    DataPacket data (DataPacket::create(pkt.size()));

    data.annotation<annotations::Quality>()               = rtp.annotation<annotations::Quality>();
    data.annotation<annotations::Interface>().value       = rtp.annotation<annotations::Interface>().value;
    data.annotation<annotations::Timestamp>()             = rtp.annotation<annotations::Timestamp>();
    data.annotation<annotations::WirelessModulation>().id = rtp.annotation<annotations::WirelessModulation>().id;

    stats_.substitute++;
    outExtUI(data, src, dst);
}


prefix_ void senf::emu::MonitorDataFilter::handle_badFCS(RadiotapPacket const & rtPacket)
{
    stats_.badFCS++;
    rtPacket.annotation<annotations::Quality>().flags.frameCorrupt = true;
    pushSubstituteEthernet( rtPacket);
}

prefix_ bool senf::emu::MonitorDataFilter::handle_ManagementFrame(RadiotapPacket const & rtPacket, WLANPacket_MgtFrame const & mgt)
{
    if (mgt.size() < WLANPacket_MgtFrameParser::init_bytes) {
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping truncated management frame") );
        stats_.truncated++;
        return false;
    }

    stats_.management++;
    outExtUI(mgt);

    return true;
}

prefix_ bool senf::emu::MonitorDataFilter::handle_CtrlFrame(RadiotapPacket const & rtPacket, WLANPacket_CtrlFrame const & ctrl)
{
    if (ctrl.size() < WLANPacket_CtrlFrameParser::init_bytes) {
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping truncated ctrl frame") );
        stats_.truncated++;
        return false;
    }

    stats_.control++;
    outExtUI(ctrl);

    return true;
}

prefix_ void senf::emu::MonitorDataFilter::handle_NonQoSData(RadiotapPacket const & rtPacket)
{
    WLANPacket_DataFrame const & data (rtPacket.next<WLANPacket_DataFrame>(senf::nothrow));
    if (!data) {
        pushSubstituteEthernet( rtPacket);
        return;
    }

    if (data.size() < WLANPacket_DataFrameParser::init_bytes) {
        SENF_LOG( (WlanLogArea) (senf::log::VERBOSE) ("(" << id_ << ") dropping truncated non-QoS data frame") );
        stats_.truncated++;
        return;
    }

    stats_.data++;
    outExtUI(data);
}


prefix_ void senf::emu::MonitorDataFilter::handle_NonDataFrame(RadiotapPacket const & rtPacket)
{
    // 
    // This might be somewhat less efficient, but we should just see a few frames per second (as we filter ACKs via BPF) 
    //
    WLANPacket_MgtFrame const & mgt (rtPacket.next<WLANPacket_MgtFrame>(senf::nothrow));
    if (mgt) {
        if (!handle_ManagementFrame( rtPacket, mgt)) {
            pushSubstituteEthernet( rtPacket);
        }
        return;
    }
    WLANPacket_CtrlFrame const & ctrl (rtPacket.next<WLANPacket_CtrlFrame>(senf::nothrow));
    if (ctrl) {
        if (!handle_CtrlFrame( rtPacket, ctrl)) {
            pushSubstituteEthernet( rtPacket);
        }
        return;
    }

    // If we make it here, the frame type is not known (most likely, it is encrypted!)
    try {
        DataPacket const & data (rtPacket.parseNextAs<DataPacket>());
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

        if (SENF_LIKELY(frequency_ and rtParser.channelOptionsPresent())) {
            if (SENF_UNLIKELY(rtParser.channelOptions().freq() != frequency_)) {
                stats_.freqMismatch++;
                return;
            }
        }

        // Set Annotations: incoming interface
        rtPacket.annotation<annotations::Interface>().value = id_;

        // set the rx timestamp. Careful: this assumes that we are using an MMAP source !
        rtPacket.annotation<annotations::Timestamp>().fromQueueBuffer(*(rtPacket.annotation<senf::ppi::QueueBufferAnnotation>().value));

        // Set annotations: Quality
        {
            annotations::Quality const & q (rtPacket.annotation<annotations::Quality>());
            q.rssi  = short(rtParser.dbmAntennaSignal());
            q.noise = (rtParser.dbmAntennaNoisePresent() ? short(rtParser.dbmAntennaNoise()) : short(DEFAULT_WLAN_NOISE));
            q.snr   = short(q.rssi - q.noise);
            q.flags.frameLength = rtPacket.size() - rtParser.length();
            // ampduStatus is present ==> frame is aggregated
            stats_.aggregated += q.flags.frameAggregated = rtParser.ampduStatusPresent();
        }

        // determine MCS (rate)
        std::uint32_t modulationId;
        if (SENF_LIKELY(rtParser.mcsPresent())) {
            stats_.ht++;
            modulationId = rtPacket.annotation<annotations::WirelessModulation>().id = modulationRegistry_.parameterIdByMCS_HT(
                rtParser.mcs().mcsIndex(),
                rtParser.bandwidth(),
                rtParser.mcs().guardInterval() );
        }
        else if (rtParser.vhtPresent()) {
            stats_.vht++;
            modulationId = rtPacket.annotation<annotations::WirelessModulation>().id = modulationRegistry_.parameterIdByMCS_VHT(
                rtParser.vht().mcs_user0(),
                rtParser.vht().nss_user0(),
                rtParser.bandwidth(),
                rtParser.vht().guardInterval() );
        }
        else if (rtParser.ratePresent()) {
            stats_.legacy++;
            modulationId = rtPacket.annotation<annotations::WirelessModulation>().id = modulationRegistry_.parameterIdByLegacyRate( rtParser.rate() * 500);
        }
        else {
            stats_.unknownMCS++;
            modulationId = 0;
            if (dropUnknownMCS_) {
                SENF_LOG( (WlanLogArea) (senf::log::VERBOSE)
                          ( "(" << id_ << ") dropping packet with unknown datarate "));
                return;
            }
        }

        if (airTime_ and modulationId) {
            emu::annotations::Quality const & q (rtPacket.annotation<emu::annotations::Quality>());
            // airtime in us
            q.airTime = (q.flags.frameLength * 8 * 1000) / modulationRegistry_.findModulationById(modulationId).rate;
            if (SENF_UNLIKELY(!q.flags.frameAggregated)) {
                q.airTime += 20;  // preamble (minimum)
            } else {
                if (SENF_UNLIKELY(ampduRefNo_ != rtParser.ampduStatus().referenceNumber())) {
                    // new AMPDU
                    ampduRefNo_ = rtParser.ampduStatus().referenceNumber();
                    q.airTime += 20;  // preamble (minimum). Note: We add this to the first packet in the AMPDU.
                }
            }
        }
        
        // Catch frames with bad FCS...
        if (SENF_UNLIKELY(rtParser.flags().badFCS())) {
            handle_badFCS(rtPacket);
            return;
        }
        
        unsigned radiotapLength (senf::bytes(rtParser));
        senf::WLANPacket_DataFrameParser wlan (rtPacket.data().begin() + radiotapLength, &rtPacket.data());
        if (SENF_UNLIKELY(wlan.type() != 2)) {
            // promisc_ check is handle inside pushSubstitute()
            handle_NonDataFrame(rtPacket);
            return;
        }

        if (SENF_UNLIKELY(wlan.subtype() != 8)) {
            // promisc_ check is handle inside handle_NonQoSData()
            handle_NonQoSData(rtPacket);
            return;
        }
        
        senf::MACAddress src (wlan.sourceAddress());
        senf::MACAddress dst (wlan.destinationAddress());

        // TSFT Histogram
        if (SENF_UNLIKELY(tsftHistogram_.enabled())) {
            tsftHistogram_.update( src, rtParser.tsft());
        }
 
        if (SENF_UNLIKELY(dst != id_ && !dst.multicast() && !promisc_))
            return;

        // We have 16 unused bits in the 'key' since the MAC address is only 48 bits.
        // So let's use some of them to indicate the TID or the default (no QoS Data) seqNo sequence (we use 0xff for that)
        // If we have a QoSData frame, we need to keep separate seq# counters per TID (bits 0...3) for UNICAST traffic
        // If we have a regular Data frame, we use the same queue as for bcast and management frames
        unsigned seqNo (wlan.sequenceNumber());
        SequenceNumberMap::key_type key;
        wlan.sourceAddress().hash(&key, wlan.qosField());

        // flag retransmission
        {
            stats_.retries += rtPacket.annotation<annotations::Quality>().flags.frameRetransmitted = wlan.retry();
        }

        senf::EthernetPacket eth (
            rtPacket.replaceAs<senf::EthernetPacket>(
                radiotapLength + senf::bytes(wlan) + senf::LlcSnapPacketParser::fixed_bytes
                - senf::EthernetPacketParser::fixed_bytes, signed(rtParser.has_fcs()) * -4));
        
        eth->source() << src;
        eth->destination() << dst;
        
        SequenceNumberMap::iterator i (sequenceNumberMap_.find(key));
        if (SENF_LIKELY(i != sequenceNumberMap_.end())) {
            int delta (seqNoDelta(seqNo, i->second.number));
            if (SENF_LIKELY(delta == 1)) {
                i->second.number = seqNo;
                i->second.last = senf::scheduler::now();
            }
            else if ((i->second.last + senf::ClockService::milliseconds(SEQ_EXPIRE)) < (senf::scheduler::now())) {
                stats_.seqNoExpired++;
                flushQueue(key);
                sequenceNumberMap_.emplace(std::make_pair(key, SequenceNumber( seqNo, senf::scheduler::now())));
            }
            else if (delta > 1) {
                handleReorderedPacket(key, i->second.number, seqNo, eth);
                return;
            } 
            else if (delta < -REORDER_MAX) {
                stats_.reorderResync++;
                flushQueue(key);
                sequenceNumberMap_.emplace(std::make_pair(key, SequenceNumber( seqNo, senf::scheduler::now())));
            }
            else {
                handle_DuplicateFrame(eth);
                return;
            }
        } else {
            sequenceNumberMap_.emplace(std::make_pair(key, SequenceNumber( seqNo, senf::scheduler::now())));
        }

        outData(eth);
    }
    catch (senf::TruncatedPacketException const &) {
       stats_.truncated++;
    }
}

prefix_ void senf::emu::MonitorDataFilter::outExtUI(Packet const & pkt, senf::MACAddress const & src_, senf::MACAddress const & dst_)
{
    output(prependAnnotationsPacket(pkt, src_, dst_));
}

prefix_ void senf::emu::MonitorDataFilter::outData(senf::EthernetPacket const & eth)
{
    stats_.data++;
    if (SENF_UNLIKELY(annotate_))
        outExtUI(eth);
    else
        output(eth);
}

prefix_ void senf::emu::MonitorDataFilter::dumpState(std::ostream & os)
    const
{
    os << "reorderMap.size() = " << reorderMap_.size() << ", reorderQueueTimer is " << senf::str(reorderQueueTimer_.enabled()) << std::endl;
    for (auto const & seqNo : sequenceNumberMap_) {
        ReorderMap::const_iterator i (reorderMap_.find(seqNo.first));
        os << std::hex << "0x" << seqNo.first << std::dec << " => " << seqNo.second.number << " " << senf::ClockService::in_milliseconds(senf::scheduler::now() - seqNo.second.last) << "ms"
           << ", queueSize: " << (i==reorderMap_.end() ? "(none)" : senf::str(i->second.queue.size())) << std::endl;
    } 
}

prefix_ void senf::emu::MonitorDataFilter::requestPlain()
{
    senf::EthernetPacket const & eth (input_plain());

    stats_.received++;
    
    eth.annotation<annotations::Interface>().value = id_;
    
    // set the rx timestamp. Careful: this assumes that we are using an MMAP source !
    eth.annotation<annotations::Timestamp>().fromQueueBuffer(*(eth.annotation<senf::ppi::QueueBufferAnnotation>().value));    
    {
        emu::annotations::Quality const & q (eth.annotation<emu::annotations::Quality>());
        q.rssi  = 0;  // for now, we report the maximum signal 'quality'
        q.noise = DEFAULT_WLAN_NOISE;
        q.snr   = short(q.rssi - q.noise);
        q.flags.frameLength = eth.size();
    }

    stats_.data++;
    
    output(eth);
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
