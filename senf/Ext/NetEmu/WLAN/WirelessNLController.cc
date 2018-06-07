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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief WirelessNLController non-inline non-template implementation */

#include "WirelessNLController.hh"

// Custom includes
#include <algorithm>
#include <fstream>
#include <net/if.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <senf/Utils/Cpp11Support/cast.hh>
#include <senf/Utils/Range.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/algorithm.hh>
#include <senf/Utils/membind.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define FOREACH_ID(x) BOOST_PP_CAT(x, __LINE__)
#define FOREACH_NESTED_ATTR(attr, nla)                                              \
    if (nla)                                                                        \
        for (int FOREACH_ID(_remaining) = nla_len(nla),                             \
                 FOREACH_ID(_loop_done) = 0;                                        \
             FOREACH_ID(_loop_done)==0; ++FOREACH_ID(_loop_done))                   \
            for (nlattr * attr = nla_attr_data(nla);                                \
                 nla_ok(attr, FOREACH_ID(_remaining));                              \
                 attr = nla_next(attr, &(FOREACH_ID(_remaining))))

#undef NLA_PUT

#define NLA_PUT(msg, attrtype, attrlen, data)                                       \
    do {                                                                            \
        int r;                                                                      \
        if ((r = nla_put(msg.get(), attrtype, attrlen, data)) < 0)                  \
            throw NetlinkException(r, "Failed to build the netlink message");       \
    } while(0)

#define NLA_PUT_NESTED(msg, attrtype, nestedMsg)                                    \
    do {                                                                            \
        int r;                                                                      \
        if ((r = nla_put_nested(msg.get(), attrtype, nestedMsg.get())) < 0)         \
            throw NetlinkException(r, "Failed to build the netlink message");       \
    } while(0)

#define KHZ_TO_MHZ(freq) ((freq) / 1000)
#define MHZ_TO_KHZ(freq) ((freq) * 1000)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    template <class T>
    T & get_optional_value(boost::optional<T> & opt)
    {
        if (not opt)
            opt = T();
        return opt.get();
    }

    static int static_netlink_cb(nl_msg * msg, void * arg) {
        auto * cb (reinterpret_cast<senf::emu::WirelessNLController::NetlinkMsgCallback *>(arg));
        return (*cb)(msg);
    }

    static nlattr * nla_attr_data(const nlattr * nla) {
        return reinterpret_cast<nlattr *>(nla_data(nla));
    }

    struct nl_nested_attr_ptr
    {
        typedef boost::shared_ptr<nl_msg> nl_msg_ptr;
        nl_msg_ptr msg;
        nlattr * attr;

        nl_nested_attr_ptr(nl_msg_ptr msg_, int attrtype)
            : msg(msg_), attr(nla_nest_start(msg.get(), attrtype))
        {
            if (!attr) throw senf::SystemException( "Failed to build netlink message", ENOBUFS);
        }
        ~nl_nested_attr_ptr()
        {
            nla_nest_end(msg.get(), attr);
        }

    };

    struct NLAttributePolicies
    {
        static nla_policy freqPolicy[NL80211_FREQUENCY_ATTR_MAX + 1];
        static nla_policy ratePolicy[NL80211_BITRATE_ATTR_MAX + 1];
        static nla_policy surveyPolicy[NL80211_SURVEY_INFO_MAX + 1];
        static nla_policy regRulePolicy[NL80211_FREQUENCY_ATTR_MAX + 1];
        static nla_policy bssPolicy[NL80211_BSS_MAX + 1];

        static bool init() {
            memset( freqPolicy, 0, sizeof(freqPolicy));
            freqPolicy[NL80211_FREQUENCY_ATTR_FREQ]        .type = NLA_U32;
            freqPolicy[NL80211_FREQUENCY_ATTR_DISABLED]    .type = NLA_FLAG;
            freqPolicy[NL80211_FREQUENCY_ATTR_PASSIVE_SCAN].type = NLA_FLAG;
            freqPolicy[NL80211_FREQUENCY_ATTR_NO_IBSS]     .type = NLA_FLAG;
            freqPolicy[NL80211_FREQUENCY_ATTR_RADAR]       .type = NLA_FLAG;
            freqPolicy[NL80211_FREQUENCY_ATTR_MAX_TX_POWER].type = NLA_U32;
            freqPolicy[NL80211_FREQUENCY_ATTR_DFS_STATE]   .type = NLA_U32;
            freqPolicy[NL80211_FREQUENCY_ATTR_DFS_CAC_TIME].type = NLA_U32;

            memset( ratePolicy, 0, sizeof(ratePolicy));
            ratePolicy[NL80211_BITRATE_ATTR_RATE]              .type = NLA_U32;
            ratePolicy[NL80211_BITRATE_ATTR_2GHZ_SHORTPREAMBLE].type = NLA_FLAG;

            memset( surveyPolicy, 0, sizeof(surveyPolicy));
            surveyPolicy[NL80211_SURVEY_INFO_FREQUENCY].type = NLA_U32;
            surveyPolicy[NL80211_SURVEY_INFO_NOISE]    .type = NLA_U8;

            memset( regRulePolicy, 0, sizeof(regRulePolicy));
            regRulePolicy[NL80211_ATTR_REG_RULE_FLAGS]         .type = NLA_U32;
            regRulePolicy[NL80211_ATTR_FREQ_RANGE_START]       .type = NLA_U32;
            regRulePolicy[NL80211_ATTR_FREQ_RANGE_END]         .type = NLA_U32;
            regRulePolicy[NL80211_ATTR_FREQ_RANGE_MAX_BW]      .type = NLA_U32;
            regRulePolicy[NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN].type = NLA_U32;
            regRulePolicy[NL80211_ATTR_POWER_RULE_MAX_EIRP]    .type = NLA_U32;
            regRulePolicy[NL80211_ATTR_DFS_CAC_TIME]           .type = NLA_U32;

            memset( bssPolicy, 0, sizeof(bssPolicy));
            bssPolicy[NL80211_BSS_TSF].type = NLA_U64;
            bssPolicy[NL80211_BSS_FREQUENCY].type = NLA_U32;
            //bssPolicy[NL80211_BSS_BSSID] = {};
            bssPolicy[NL80211_BSS_BEACON_INTERVAL].type = NLA_U16;
            bssPolicy[NL80211_BSS_CAPABILITY].type = NLA_U16;
            //bssPolicy[NL80211_BSS_INFORMATION_ELEMENTS] = { };
            bssPolicy[NL80211_BSS_SIGNAL_MBM].type = NLA_U32;
            bssPolicy[NL80211_BSS_SIGNAL_UNSPEC].type = NLA_U8;
            bssPolicy[NL80211_BSS_STATUS].type = NLA_U32;
            bssPolicy[NL80211_BSS_SEEN_MS_AGO].type = NLA_U32;
            //bssPolicy[NL80211_BSS_BEACON_IES] = { };

            return true;
        }
    };

    nla_policy NLAttributePolicies::freqPolicy[NL80211_FREQUENCY_ATTR_MAX + 1];
    nla_policy NLAttributePolicies::ratePolicy[NL80211_BITRATE_ATTR_MAX + 1];
    nla_policy NLAttributePolicies::surveyPolicy[NL80211_SURVEY_INFO_MAX + 1];
    nla_policy NLAttributePolicies::regRulePolicy[NL80211_FREQUENCY_ATTR_MAX + 1];
    nla_policy NLAttributePolicies::bssPolicy[NL80211_BSS_MAX + 1];
    
    bool __attribute__((unused))
        _nlAttributePolicies_initialized (NLAttributePolicies::init());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WirelessNLController

prefix_ senf::emu::WirelessNLController::WirelessNLController(std::string const & interface)
    : radarEvent(*this), regulatoryDomainChangeEvent(*this), scanEvent(*this),
      ifIndex_(0), phyIndex_(-1),
      nl_sock_(nullptr, nl_socket_free), nl_cache_(nullptr, nl_cache_free), nl_cb_(nullptr, nl_cb_put),
      nl_event_sock_(nullptr, nl_socket_free), nl_event_cb_(nullptr, nl_cb_put)
{
    ifIndex_ = if_nametoindex(interface.c_str());
    if (ifIndex_ != 0) {
        phyIndex_ = if_nameto_phy_index( interface);
        phyName_ = if_nameto_phy_name( interface);
    } else {
        phyIndex_ = phy_nametoindex( interface);
        phyName_ = interface;
    }
    if (phyIndex_ == -1)
        throw senf::SystemException( "Could not discover the index of " , EINVAL) << interface;

    init(true); // disable seqNoCheck
}

prefix_ senf::emu::WirelessNLController::WirelessNLController(bool disableSeqNoCheck)
    : radarEvent(*this), regulatoryDomainChangeEvent(*this), scanEvent(*this),
      ifIndex_(0), phyIndex_(-1),
      nl_sock_(nullptr, nl_socket_free), nl_cache_(nullptr, nl_cache_free), nl_cb_(nullptr, nl_cb_put),
      nl_event_sock_(nullptr, nl_socket_free), nl_event_cb_(nullptr, nl_cb_put)
{
    init(disableSeqNoCheck);
}

prefix_ void senf::emu::WirelessNLController::init(bool disableSeqNoCheck)
{
    memset(&survey_, 0, sizeof(survey_));
    ifaceType_ = IfaceType::Unknown;
    hasHTCapabilities_ = false;
    hasVHTCapabilities_ = false;
    callback_ = nullptr;
    netlinkMsgCb_ = senf::membind(&WirelessNLController::netlink_cb, this);
    netlinkEventCb_ = senf::membind(&WirelessNLController::netlink_event_cb, this);

    initNlSock(nl_sock_, disableSeqNoCheck);
    initNlCb(nl_sock_, nl_cb_, netlinkMsgCb_);
    initNlCache(nl_sock_, nl_cache_);

    nl80211Id_ = genl_ctrl_resolve(nl_sock_.get(), "nl80211");
    if (nl80211Id_ < 0)
        SENF_THROW_SYSTEM_EXCEPTION( "nl80211 not found");
}

prefix_ void senf::emu::WirelessNLController::initNlSock(nl_sock_ptr & sock, bool disableSeqNoCheck)
{
    sock.reset(nl_socket_alloc());
    if (not sock)
        SENF_THROW_SYSTEM_EXCEPTION( "Failed to allocate netlink socket");

    if (genl_connect(sock.get()))
        SENF_THROW_SYSTEM_EXCEPTION( "Failed to connect generic netlink socket");

    if (disableSeqNoCheck)
        nl_socket_disable_seq_check(sock.get());
    nl_socket_disable_auto_ack(sock.get());
}

prefix_ void senf::emu::WirelessNLController::initNlCache(nl_sock_ptr & sock, nl_cache_ptr & cache)
{
    nl_cache * c;
    if (genl_ctrl_alloc_cache(sock.get(), &c))
        SENF_THROW_SYSTEM_EXCEPTION( "Failed to connect generic netlink cache");
    cache.reset(c);
}

prefix_ void senf::emu::WirelessNLController::initNlCb(nl_sock_ptr & sock, nl_cb_ptr & cb, NetlinkMsgCallback & msgcb)
{
    cb.reset(nl_cb_alloc(NL_CB_DEFAULT));
    if (not cb)
        SENF_THROW_SYSTEM_EXCEPTION( "Failed to allocate netlink callbacks\n");
    nl_cb_set(cb.get(), NL_CB_VALID, NL_CB_CUSTOM, static_netlink_cb, &msgcb);
    nl_socket_set_cb(sock.get(), cb.get());
}

prefix_ int senf::emu::WirelessNLController::phyIndex()
    const
{
    return phyIndex_;
}

prefix_ std::string const & senf::emu::WirelessNLController::phyName()
    const
{
    return phyName_;
}

prefix_ senf::emu::WirelessNLController::nl_msg_ptr senf::emu::WirelessNLController::nlMsg()
{
    nl_msg_ptr msg (nlmsg_alloc(), &nlmsg_free);
    if (!msg)
        SENF_THROW_SYSTEM_EXCEPTION( "Failed to allocate netlink message");
    return msg;
}

prefix_ senf::emu::WirelessNLController::nl_msg_ptr
senf::emu::WirelessNLController::nlMsgHeader(uint8_t cmd, CmdIdBy idBy, int flags)
    const
{
    nl_msg_ptr msg (nlMsg());
    genlmsg_put( msg.get(),
            NL_AUTO_PID,    // pid
            NL_AUTO_SEQ,    // sequence number of message
            nl80211Id_,     // netlink family of nl80211
            0,              // length of user specific header
            flags,          // message flags
            cmd,            // netlink command
            0 );            // protocol version
    switch (idBy) {
    case CIB_PHY:
        if (phyIndex_ == -1)
            throw NetlinkException(NLE_INVAL, "Device index missing");
        NLA_PUT_U32( msg, NL80211_ATTR_WIPHY, phyIndex_);
        break;
    case CIB_IF:
        if (ifIndex_ == 0)
            throw NetlinkException(NLE_INVAL, "Interface index missing");
        NLA_PUT_U32( msg, NL80211_ATTR_IFINDEX, ifIndex_);
        break;
    case CIB_NONE:
        break;
    }

    return msg;
}

prefix_ void senf::emu::WirelessNLController::send_and_wait4response(nl_msg_ptr const & msg, CallbackMemPtr cb)
{
    callback_ = cb;
    int r;
    if ((r = nl_send_auto_complete(nl_sock_.get(), msg.get())) > 0) {
        if ((r = nl_recvmsgs_default( nl_sock_.get())) != 0)
            throw NetlinkException(r);
    } else {
        throw NetlinkException(r);
    }
    callback_ = nullptr;
}

prefix_ void senf::emu::WirelessNLController::nlPutChannelDef(nl_msg_ptr msg, frequency_type freq, ChannelMode::Enum channelMode)
{
    NLA_PUT_U32 ( msg, NL80211_ATTR_WIPHY_FREQ, KHZ_TO_MHZ(freq));
    if (channelType(channelMode) == NL80211_CHAN_NO_HT or channelType(channelMode) == NL80211_CHAN_HT20) {
        NLA_PUT_U32 ( msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE, channelType(channelMode));
    }
    else {
        NLA_PUT_U32 ( msg, NL80211_ATTR_CHANNEL_WIDTH, channelWidth(channelMode));
        NLA_PUT_U32 ( msg, NL80211_ATTR_CENTER_FREQ1, KHZ_TO_MHZ(centerFreq(freq, channelMode)));
    }
}

prefix_ int senf::emu::WirelessNLController::netlink_cb(nl_msg * msg)
{
    if (callback_)
        return (this->*callback_)(msg);
    return NL_SKIP;
}

prefix_ std::pair<senf::emu::WirelessNLController::DFSState::Enum,std::uint32_t> senf::emu::WirelessNLController::dfsState(frequency_type freq)
{
    getWiphy();
    DFSStateMap::const_iterator state (dfsStates_.find(freq));
    if (state != dfsStates_.end()) {
        return state->second;
    }
    return std::make_pair(DFSState::NoDFS, 0);
}

prefix_ void senf::emu::WirelessNLController::set_frequency(frequency_type freq, ChannelMode::Enum channelMode)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_WIPHY, CIB_PHY, NLM_F_ACK));

    nlPutChannelDef(msg, freq, channelMode);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::set_txpower(TxPowerSetting::Enum setting, unsigned int mBm)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_WIPHY, CIB_PHY, NLM_F_ACK));

    NLA_PUT_U32( msg, NL80211_ATTR_WIPHY_TX_POWER_SETTING, setting);
    if (setting != TxPowerSetting::Auto)
        NLA_PUT_U32( msg, NL80211_ATTR_WIPHY_TX_POWER_LEVEL, mBm);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::set_retryLimit(boost::uint8_t shortLimit, boost::uint8_t longLimit)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_WIPHY, CIB_PHY, NLM_F_ACK));

    NLA_PUT_U8( msg, NL80211_ATTR_WIPHY_RETRY_LONG, longLimit);
    NLA_PUT_U8( msg, NL80211_ATTR_WIPHY_RETRY_SHORT, shortLimit);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::start_radarDetection(unsigned int freq, ChannelMode::Enum channelMode)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_RADAR_DETECT, CIB_IF, NLM_F_ACK));

    nlPutChannelDef(msg, freq, channelMode);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::set_regulatory_request(std::string const & alpha2Country)
{
    if (alpha2Country.size() != 2)
        throw InvalidArgumentException("invalid alpha2 country: ") << alpha2Country;

    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_REQ_SET_REG, CIB_NONE, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_REG_ALPHA2, alpha2Country.c_str());

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::set_regulatory(RegulatoryDomain const & regDomain)
{
    if (regDomain.alpha2Country.size() != 2)
        throw InvalidArgumentException("invalid alpha2 country: ") << regDomain.alpha2Country;

    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_REG, CIB_NONE, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_REG_ALPHA2, regDomain.alpha2Country.c_str());
    NLA_PUT_U8    ( msg, NL80211_ATTR_DFS_REGION, underlying_type_cast(regDomain.dfsRegion));
    {
        nl_nested_attr_ptr msgAttr (msg, NL80211_ATTR_REG_RULES);
        unsigned i = 0;
        for (RegulatoryRule const & rule : regDomain.rules) {
            nl_nested_attr_ptr params (msg, i);  // crda-3.13 always passes i == 0 here, so let's do the same 
            NLA_PUT_U32( msg, NL80211_ATTR_REG_RULE_FLAGS,      rule.flags());
            NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_START,    rule.frequencyRangeBegin());
            NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_END,      rule.frequencyRangeEnd());
            NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_MAX_BW,   rule.maxBandwidth());
            NLA_PUT_U32( msg, NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN, rule.maxAntennaGain());
            NLA_PUT_U32( msg, NL80211_ATTR_POWER_RULE_MAX_EIRP, rule.maxEIRP());
//            NLA_PUT_U32( msg, NL80211_ATTR_DFS_CAC_TIME,        rule.cacTime());
        }
    }

    send_and_wait4response(msg);
}

prefix_ senf::emu::RegulatoryDomain senf::emu::WirelessNLController::get_regulatory()
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_GET_REG, CIB_NONE, NLM_F_ACK));

    send_and_wait4response(msg, &WirelessNLController::getRegDomain_cb);

    return regDomain_;
}

//
// should be called AFTER joinCell()
// queues: NL80211_TXQ_Q_VO, NL80211_TXQ_Q_VI, NL80211_TXQ_Q_BK, NL80211_TXQ_Q_BE;
//
prefix_ void senf::emu::WirelessNLController::set_txQueueParameters(boost::uint8_t queue,
        boost::uint16_t cwMin, boost::uint16_t cwMax, boost::uint8_t aifs, boost::uint16_t txop)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_WIPHY, CIB_IF, NLM_F_ACK));

    {
        nl_nested_attr_ptr msgAttr (msg, NL80211_ATTR_WIPHY_TXQ_PARAMS);
        nl_nested_attr_ptr params (msg, 1);
        NLA_PUT_U8 (msg, NL80211_TXQ_ATTR_QUEUE, queue);
        NLA_PUT_U16(msg, NL80211_TXQ_ATTR_CWMIN, cwMin);
        NLA_PUT_U16(msg, NL80211_TXQ_ATTR_CWMAX, cwMax);
        NLA_PUT_U8 (msg, NL80211_TXQ_ATTR_AIFS,  aifs);
        NLA_PUT_U16(msg, NL80211_TXQ_ATTR_TXOP,  txop);
    }

    send_and_wait4response(msg);
}

namespace {
    // WTF?!? NL80211_TXRATE_LEGACY requires "u8 values with 1 = 500 kbps", but
    //   NL80211_BITRATE_ATTR_RATE provides "units of 100 kbps". GRRR!
    uint8_t transform_bitrate(uint32_t r) {
        return r / 500;
    }
}

#define NLA_PUT_OPTIONAL_VECTOR(msg, attrtype, optvector)           \
    do {                                                            \
        if (optvector) NLA_PUT( msg, attrtype, optvector->size(),   \
                optvector->empty() ? NULL : &(*optvector)[0]);      \
    } while (0);

prefix_ void senf::emu::WirelessNLController::set_bitrates(BitrateParameters p)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_TX_BITRATE_MASK, CIB_IF, NLM_F_ACK));

    // copy the sets to a vector to get continuous memory
    typedef std::vector<uint8_t> LegacyBitrateVector;
    boost::optional<LegacyBitrateVector> legacy_24, legacy_5;
    typedef std::vector<BitrateParameters::MCSIndex> MCSIndexVector;
    boost::optional<MCSIndexVector> mcs_24, mcs_5;
    struct nl80211_txrate_vht txrate_vht_24, txrate_vht_5;

    if (p.legacy_24) {
        legacy_24.reset( LegacyBitrateVector(p.legacy_24->size()));
        std::transform(p.legacy_24->begin(), p.legacy_24->end(), legacy_24->begin(), transform_bitrate);
    }
    if (p.legacy_5) {
        legacy_5.reset( LegacyBitrateVector(p.legacy_5->size()));
        std::transform(p.legacy_5->begin(),  p.legacy_5->end(),  legacy_5->begin(),  transform_bitrate);
    }
    if (p.mcs_24) {
        mcs_24.reset(MCSIndexVector(p.mcs_24->size()));
        std::copy(p.mcs_24->begin(), p.mcs_24->end(), mcs_24->begin());
    }
    if (p.mcs_5) {
        mcs_5.reset(MCSIndexVector(p.mcs_5->size()));
        std::copy(p.mcs_5->begin(), p.mcs_5->end(), mcs_5->begin());
    }
    if (p.vht_mcs_table_24) {
        for (int nss = 0; nss < NL80211_VHT_NSS_MAX; nss++)
            txrate_vht_24.mcs[nss] = p.vht_mcs_table_24->at(nss).to_ulong();
    }
    if (p.vht_mcs_table_5) {
        for (int nss = 0; nss < NL80211_VHT_NSS_MAX; nss++)
            txrate_vht_5.mcs[nss] = p.vht_mcs_table_5->at(nss).to_ulong();
    }
    {
        nl_nested_attr_ptr msgAttr (msg, NL80211_ATTR_TX_RATES);
        if (legacy_24 or mcs_24 or p.vht_mcs_table_24 or p.gi_24) {
            nl_nested_attr_ptr bandAttr (msg, NL80211_BAND_2GHZ);
            NLA_PUT_OPTIONAL_VECTOR( msg, NL80211_TXRATE_LEGACY, legacy_24);
            NLA_PUT_OPTIONAL_VECTOR( msg, NL80211_TXRATE_HT,     mcs_24);
            if (p.vht_mcs_table_24)
                NLA_PUT            ( msg, NL80211_TXRATE_VHT,    sizeof(txrate_vht_24), &txrate_vht_24);
            if (p.gi_24)
                NLA_PUT_U8         ( msg, NL80211_TXRATE_GI,     p.gi_24.get());
        }
        if (legacy_5 or mcs_5 or p.vht_mcs_table_5 or p.gi_5) {
            nl_nested_attr_ptr bandAttr (msg, NL80211_BAND_5GHZ);
            NLA_PUT_OPTIONAL_VECTOR( msg, NL80211_TXRATE_LEGACY, legacy_5);
            NLA_PUT_OPTIONAL_VECTOR( msg, NL80211_TXRATE_HT,     mcs_5);
            if (p.vht_mcs_table_5)
                NLA_PUT            ( msg, NL80211_TXRATE_VHT,    sizeof(txrate_vht_5), &txrate_vht_5);
            if (p.gi_5)
                NLA_PUT_U8         ( msg, NL80211_TXRATE_GI,     p.gi_5.get());
        }
    }

    send_and_wait4response(msg);
}

prefix_ std::set<senf::emu::WirelessNLController::Band_t> const & senf::emu::WirelessNLController::supportedBands()
{
    getWiphy();
    return supportedBands_;
}

prefix_ senf::emu::BitrateParameters const & senf::emu::WirelessNLController::bitrates()
{
    getWiphy();
    return bitrates_;
}

prefix_ senf::emu::WirelessNLController::FrequencyRange senf::emu::WirelessNLController::frequencies()
{
    getWiphy();
    return senf::make_transform_range(frequencies_,
            ::__gnu_cxx::select2nd<Frequencies::value_type>());
}

prefix_ senf::emu::WirelessNLController::FrequencyRange senf::emu::WirelessNLController::frequencies(Band_t band)
{
    getWiphy();
    return senf::make_transform_range(frequencies_.equal_range(band),
            ::__gnu_cxx::select2nd<Frequencies::value_type>());
}

namespace {
#   define BIT(x) (1ULL<<(x))
    using namespace senf::emu;

    HTCapabilitiesInfo parseHTCapabilities(uint16_t flags)
    {
        HTCapabilitiesInfo capInfo;
        capInfo.rxLDPC = flags & BIT(0);
        capInfo.channelWidth = HTCapabilitiesInfo::ChannelWidth( flags & BIT(1));
        capInfo.smPowerSave = HTCapabilitiesInfo::SMPowerSave( (flags >> 2) & 0x3);
        capInfo.rxGreenfield = flags & BIT(4);
        capInfo.rxHT20SGI = flags & BIT(5);
        capInfo.rxHT40SGI = flags & BIT(6);
        capInfo.txSTBC = flags & BIT(7);
        capInfo.rxSTBC = HTCapabilitiesInfo::RXSTBC( (flags >> 8) & 0x3);
        capInfo.delayedBlockAck = flags & BIT(10);
        capInfo.maxAMSDULength = HTCapabilitiesInfo::MaxAMSDULength( flags & BIT(11));
        capInfo.ht40DSSS_CCK = flags & BIT(12);
        // BIT(13) is reserved
        capInfo.fortyMHzIntolerant = flags & BIT(14);
        capInfo.lSIG_TXOP_Protection = flags & BIT(15);
        return capInfo;
    }

    VHTCapabilitiesInfo parseVHTCapabilities(uint32_t flags)
    {
        VHTCapabilitiesInfo capInfo;
        capInfo.maxMPDULength = VHTCapabilitiesInfo::MaxMPDULength(flags & 3);
        capInfo.supportedChannelWidth = VHTCapabilitiesInfo::SupportedChannelWidth((flags >> 2) & 3);
        capInfo.rxLDPC = flags & BIT(4);
        capInfo.shortGIfor80MHz = flags & BIT(5);
        capInfo.shortGIfor160_8080MHz = flags & BIT(6);
        capInfo.txSTBC = flags & BIT(7);
        capInfo.suBeamformer = flags & BIT(11);
        capInfo.suBeamformee = flags & BIT(12);
        capInfo.suBeamformer = flags & BIT(19);
        capInfo.suBeamformee = flags & BIT(20);
        capInfo.vhtTxOpPowerSave = flags & BIT(21);
        capInfo.htc_vhtCapable = flags & BIT(22);
        capInfo.rxAntennaPatternConsistency = flags & BIT(28);
        capInfo.txAntennaPatternConsistency = flags & BIT(29);
        return capInfo;
    };

#   undef BIT
}

#define GET_RULE_ATTR_OR_SKIP( var, attr )      \
    std::uint32_t var;                          \
    if (not ruleAttr[attr]) continue;           \
    var = nla_get_u32(ruleAttr[attr]);

prefix_ int senf::emu::WirelessNLController::getRegDomain_cb(nl_msg * msg)
{
    nlattr * ruleAttr[NL80211_FREQUENCY_ATTR_MAX + 1];
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];

    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    regDomain_ = RegulatoryDomain();

    if (not msgAttr[NL80211_ATTR_REG_ALPHA2] or not msgAttr[NL80211_ATTR_REG_RULES])
        return NL_SKIP;

    char * alpha2 = reinterpret_cast<char *>(nla_data(msgAttr[NL80211_ATTR_REG_ALPHA2]));
    regDomain_.alpha2Country = std::string(alpha2, 2);

    if (msgAttr[NL80211_ATTR_DFS_REGION])
        regDomain_.dfsRegion = RegulatoryDomain::DFSRegion(nla_get_u8(msgAttr[NL80211_ATTR_DFS_REGION]));
    else
        regDomain_.dfsRegion = RegulatoryDomain::DFSRegion::Unset;

    FOREACH_NESTED_ATTR( rule, msgAttr[NL80211_ATTR_REG_RULES] ) {
        nla_parse(ruleAttr, NL80211_FREQUENCY_ATTR_MAX, nla_attr_data(rule), nla_len(rule), NLAttributePolicies::regRulePolicy);

        GET_RULE_ATTR_OR_SKIP( flags,            NL80211_ATTR_REG_RULE_FLAGS          );
        GET_RULE_ATTR_OR_SKIP( start_freq_khz,   NL80211_ATTR_FREQ_RANGE_START        );
        GET_RULE_ATTR_OR_SKIP( end_freq_khz,     NL80211_ATTR_FREQ_RANGE_END          );
        GET_RULE_ATTR_OR_SKIP( max_bw_khz,       NL80211_ATTR_FREQ_RANGE_MAX_BW       );
        GET_RULE_ATTR_OR_SKIP( max_ant_gain_mbi, NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN );
        GET_RULE_ATTR_OR_SKIP( max_eirp_mbm,     NL80211_ATTR_POWER_RULE_MAX_EIRP     );
        GET_RULE_ATTR_OR_SKIP( cac_time_ms,      NL80211_ATTR_DFS_CAC_TIME            );

        regDomain_.rules.insert( RegulatoryRule()
                .frequencyRange(start_freq_khz, end_freq_khz)
                .maxBandwidth(max_bw_khz)
                .maxAntennaGain(max_ant_gain_mbi)
                .maxEIRP(max_eirp_mbm)
                .cacTime(cac_time_ms)
                .flags(flags) );
    }

    return NL_OK;
}

prefix_ int senf::emu::WirelessNLController::getWiphy_cb(nl_msg * msg)
{
//    std::cout << "getWiphy_cb " << int(firstMutlipartMsg_) << std::endl;
//    nl_msg_dump(msg, stdout);
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];
    nlattr * bandAttr[NL80211_BAND_ATTR_MAX + 1];
    nlattr * freqAttr[NL80211_FREQUENCY_ATTR_MAX + 1];
    nlattr * rateAttr[NL80211_BITRATE_ATTR_MAX + 1];
    uint16_t htCapa = 0;
    uint32_t vhtCapa = 0;

    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

//    if (! msgAttr[NL80211_ATTR_WIPHY_BANDS])
//        return NL_SKIP;

    if (firstMutlipartMsg_) {
        firstMutlipartMsg_ = false;
        bitrates_ = BitrateParameters();
        frequencies_.clear();
        supportedBands_.clear();
        dfsStates_.clear();
        bandtype_ = -1;
        tmpMCSRates_.reset();
        tmpLegacyRates_.reset();
        coverageClass_ = 0;
    }

    //    detail::dumpMsgAttributes(msgAttr, std::cout);

    bool firstBand (true);
    FOREACH_NESTED_ATTR( band, msgAttr[NL80211_ATTR_WIPHY_BANDS] ) {
        nla_parse(bandAttr, NL80211_BAND_ATTR_MAX, nla_attr_data(band), nla_len(band), NULL);

//        detail::dumpBandAttributes(bandAttr, std::cout);

        // iterate over frequencies
        FOREACH_NESTED_ATTR( freq, bandAttr[NL80211_BAND_ATTR_FREQS] ) {
            nla_parse(freqAttr, NL80211_FREQUENCY_ATTR_MAX, nla_attr_data(freq), nla_len(freq),
                    NLAttributePolicies::freqPolicy);
            if (not freqAttr[NL80211_FREQUENCY_ATTR_FREQ])
                continue;
            uint32_t f (nla_get_u32(freqAttr[NL80211_FREQUENCY_ATTR_FREQ]));
//            if (freqAttr[NL80211_FREQUENCY_ATTR_RADAR])
//                std::cout << f << " RADAR\n";
            bandtype_ = f < 3000 ? BAND_2GHZ : BAND_5GHZ;
            if (freqAttr[NL80211_FREQUENCY_ATTR_DISABLED])
                continue;
            frequencies_.insert( std::make_pair( Band_t(bandtype_), MHZ_TO_KHZ(f)));

//            detail::dumpFreqAttributes(freqAttr, std::cout);

            if (freqAttr[NL80211_FREQUENCY_ATTR_DFS_STATE]) {
                uint32_t state = nla_get_u32(freqAttr[NL80211_FREQUENCY_ATTR_DFS_STATE]);
                uint32_t time  = nla_get_u32(freqAttr[NL80211_FREQUENCY_ATTR_DFS_TIME]);
                dfsStates_[MHZ_TO_KHZ(f)] = std::make_pair(DFSState::Enum(state), time);
            }
        }

        if (bandtype_ != -1) {
            supportedBands_.insert( Band_t(bandtype_));
            switch (bandtype_) {
            case BAND_2GHZ:
                if (tmpLegacyRates_) bitrates_.legacy_24 = tmpLegacyRates_;
                if (tmpMCSRates_) bitrates_.mcs_24 = tmpMCSRates_;
                break;
            case BAND_5GHZ:
                if (tmpLegacyRates_) bitrates_.legacy_5 = tmpLegacyRates_;
                if (tmpMCSRates_) bitrates_.mcs_5 = tmpMCSRates_;
                break;
            }
        }

        // iterate over bitrates
        FOREACH_NESTED_ATTR( rate, bandAttr[NL80211_BAND_ATTR_RATES] ) {
            nla_parse(rateAttr, NL80211_BITRATE_ATTR_MAX, nla_attr_data(rate), nla_len(rate),
                    NLAttributePolicies::ratePolicy);
            if (rateAttr[NL80211_BITRATE_ATTR_RATE]) {
                uint32_t r (nla_get_u32(rateAttr[NL80211_BITRATE_ATTR_RATE]));
                switch (bandtype_) {
                case -1:
                    get_optional_value(tmpLegacyRates_).insert( r*100);
                    break;
                case BAND_2GHZ:
                    get_optional_value(bitrates_.legacy_24).insert( r*100);
                    break;
                case BAND_5GHZ:
                    get_optional_value(bitrates_.legacy_5).insert( r*100);
                    break;
                }
            }
        }

        // to simplify the HT capabilities handling we assume that all bands have the same capabilities
        // otherwise log an important message and use the first one
        if (bandAttr[NL80211_BAND_ATTR_HT_CAPA]) {
            uint16_t bandHTCapa (nla_get_u16( bandAttr[NL80211_BAND_ATTR_HT_CAPA]));
            if (firstBand) {
                htCapabilities_ = parseHTCapabilities(bandHTCapa);
                hasHTCapabilities_ = true;
                htCapa = bandHTCapa;
            }
            if (bandHTCapa != htCapa)
                SENF_LOG((senf::log::IMPORTANT) ("not all bands have the same HT Capabilities; using only the first one!"));
        }
        if (bandAttr[NL80211_BAND_ATTR_VHT_CAPA]) {
            uint32_t bandVHTCapa (nla_get_u32( bandAttr[NL80211_BAND_ATTR_VHT_CAPA]));
            if (firstBand) {
                vhtCapabilities_ = parseVHTCapabilities(bandVHTCapa);
                hasVHTCapabilities_ = true;
                vhtCapa = bandVHTCapa;
            }
            if (bandVHTCapa != vhtCapa)
                SENF_LOG((senf::log::IMPORTANT) ("not all bands have the same VHT Capabilities; using only the first one!"));
        }
        if (bandAttr[NL80211_BAND_ATTR_VHT_MCS_SET]) {
            unsigned char * mcs (reinterpret_cast<unsigned char*>(nla_data(bandAttr[NL80211_BAND_ATTR_VHT_MCS_SET])));
            boost::uint16_t tmp = mcs[0] | (mcs[1] << 8);
            BitrateParameters::VHT_MCSBitmapTable & mcsBitmapTable (get_optional_value(
                    bandtype_ == BAND_2GHZ ? bitrates_.vht_mcs_table_24 : bitrates_.vht_mcs_table_5));
            for (int nss = 0; nss < NL80211_VHT_NSS_MAX; nss++) {
                switch ((tmp >> (nss * 2)) & 3) {
                case 0:
                    mcsBitmapTable[nss] = 0xff;   // MCS 0-7
                    break;
                case 1:
                    mcsBitmapTable[nss] = 0x1ff;  // MCS 0-8
                    break;
                case 2:
                    mcsBitmapTable[nss] = 0x3ff;  // MCS 0-9
                    break;
                case 3:
                    break;  /* not supported */
                }
            }
        }
        if (bandAttr[NL80211_BAND_ATTR_HT_MCS_SET] && nla_len(bandAttr[NL80211_BAND_ATTR_HT_MCS_SET]) == 16) {
            unsigned char * mcs (reinterpret_cast<unsigned char*>(nla_data(bandAttr[NL80211_BAND_ATTR_HT_MCS_SET])));
            if (!(mcs[12] & (1 << 0))) {  // tx_mcs_set_defined=0
                SENF_LOG((senf::log::IMPORTANT) ("HT RX *only* MCS rate indexes not supported"));
                continue;
            }
            if (mcs[12] & (1 << 1)) {  // tx_rx_mcs_set_not_equal=1
                SENF_LOG((senf::log::IMPORTANT) ("unequal HT RX/TX MCS rate indexes not supported"));
                continue;
            }
            for (unsigned mcs_bit = 0; mcs_bit <= 76; ++mcs_bit) {
                if (!!(mcs[mcs_bit/8] & (1 << mcs_bit % 8))) {
                    switch (bandtype_) {
                    case -1:
                        get_optional_value(tmpMCSRates_).insert( mcs_bit);
                        break;
                    case BAND_2GHZ:
                        get_optional_value(bitrates_.mcs_24).insert( mcs_bit);
                        break;
                    case BAND_5GHZ:
                        get_optional_value(bitrates_.mcs_5).insert( mcs_bit);
                        break;
                    }
                }
            }
        }

        firstBand = false;
    }

    if (msgAttr[NL80211_ATTR_WIPHY_COVERAGE_CLASS]) {
        coverageClass_ = nla_get_u8(msgAttr[NL80211_ATTR_WIPHY_COVERAGE_CLASS]);
    }

    return NL_OK;
}

prefix_ void senf::emu::WirelessNLController::getWiphy()
{
    firstMutlipartMsg_ = true;

    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_GET_WIPHY, CIB_PHY, NLM_F_DUMP));
    NLA_PUT_FLAG(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);

    send_and_wait4response(msg, &WirelessNLController::getWiphy_cb);
}

prefix_ int senf::emu::WirelessNLController::getSurvey_cb(nl_msg * msg)
{
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];
    nlattr * sinfo[NL80211_SURVEY_INFO_MAX + 1];

    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    if (not msgAttr[NL80211_ATTR_SURVEY_INFO])
        return NL_SKIP;

    if (nla_parse_nested(sinfo, NL80211_SURVEY_INFO_MAX, msgAttr[NL80211_ATTR_SURVEY_INFO],
            NLAttributePolicies::surveyPolicy) != 0)
        return NL_SKIP;

    if (sinfo[NL80211_SURVEY_INFO_FREQUENCY]) {
        if (firstMutlipartMsg_ or sinfo[NL80211_SURVEY_INFO_IN_USE]) {
            survey_.frequency = MHZ_TO_KHZ( nla_get_u32(sinfo[NL80211_SURVEY_INFO_FREQUENCY]));
            firstMutlipartMsg_ = false;
        }
    }

    if (sinfo[NL80211_SURVEY_INFO_NOISE])
        survey_.noise = nla_get_u8(sinfo[NL80211_SURVEY_INFO_NOISE]);
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME])
        survey_.channelTime = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME]);
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY])
        survey_.channelTimeBusy = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY]);
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY])
        survey_.channelTimeExtBusy = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY]);
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX])
        survey_.channelTimeRx = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX]);
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX])
        survey_.channelTimeTx = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX]);

    return NL_OK;
}

prefix_ senf::emu::WirelessNLController::Survey const & senf::emu::WirelessNLController::survey()
{
    firstMutlipartMsg_ = true;
    memset(&survey_, 0, sizeof(survey_));
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_GET_SURVEY, CIB_IF, NLM_F_DUMP));
    send_and_wait4response(msg, &WirelessNLController::getSurvey_cb);
    return survey_;
}

prefix_ senf::emu::WirelessNLController::frequency_type senf::emu::WirelessNLController::frequency()
{
    return survey().frequency;
}

prefix_ std::string senf::emu::WirelessNLController::IfaceType::str(Enum type)
{
    switch (type) {
    case Unspecified: return "unspecified";
    case AdHoc:       return "IBSS";
    case Station:     return "managed";
    case AP:          return "AP";
    case APVLan:      return "AP/VLAN";
    case WDS:         return "WDS";
    case Monitor:     return "monitor";
    case MeshPoint:   return "mesh point";
    case Unknown:     return "unknown mode";
    }
    return "Unknown mode (" + senf::str(type) + ")";
}

prefix_ std::string senf::emu::WirelessNLController::DFSState::str(Enum state)
{
    switch (state) {
    case NoDFS:       return "NoDFS";
    case Usable:      return "Usable";
    case Unavailable: return "Unavailable";
    case Available:   return "Available";
    }
    return "Unknown state (" + senf::str(state) + ")";
}

prefix_ int senf::emu::WirelessNLController::getIface_cb(nl_msg * msg)
{
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];

    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    if (msgAttr[NL80211_ATTR_IFTYPE]) {
        uint32_t type (nla_get_u32(msgAttr[NL80211_ATTR_IFTYPE]));
        ifaceType_ = type < IfaceType::Unknown ? IfaceType::Enum(type) : IfaceType::Unknown;
    }
    return NL_OK;
}

prefix_ senf::emu::WirelessNLController::IfaceType::Enum senf::emu::WirelessNLController::type()
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_GET_INTERFACE, CIB_IF));
    send_and_wait4response(msg, &WirelessNLController::getIface_cb);
    return ifaceType_;
}

prefix_ void senf::emu::WirelessNLController::add_monInterface(std::string const & name, int flags)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_NEW_INTERFACE, CIB_PHY, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_IFNAME, name.c_str());
    NLA_PUT_U32   ( msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_MONITOR);
    nl_msg_ptr flagMsg (nlMsg());
    for (int flag = 0; flag <= NL80211_MNTR_FLAG_MAX; ++flag) {
        if (flags & (1 << flag))
            NLA_PUT_FLAG(flagMsg, flag);
    }
    NLA_PUT_NESTED( msg, NL80211_ATTR_MNTR_FLAGS, flagMsg);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::add_adhocInterface(std::string const & name)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_NEW_INTERFACE, CIB_PHY, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_IFNAME, name.c_str());
    NLA_PUT_U32   ( msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_ADHOC);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::add_apInterface(std::string const & name)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_NEW_INTERFACE, CIB_PHY, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_IFNAME, name.c_str());
    NLA_PUT_U32   ( msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_AP);

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::add_meshInterface(std::string const & name, std::string const & meshId)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_NEW_INTERFACE, CIB_PHY, NLM_F_ACK));

    NLA_PUT_STRING( msg, NL80211_ATTR_IFNAME, name.c_str());
    NLA_PUT_U32   ( msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_MESH_POINT);
    if (not meshId.empty())
        NLA_PUT( msg, NL80211_ATTR_MESH_ID, meshId.length(), meshId.c_str());

    send_and_wait4response(msg);
}


prefix_ void senf::emu::WirelessNLController::del_interface(std::string const & name)
{
    int ifindex = if_nametoindex(name.c_str());
    if (!ifindex) {
        throw senf::SystemException( "Could not discover the index of interface ", EINVAL) << name;
    }

    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_DEL_INTERFACE, CIB_NONE, NLM_F_ACK));
    NLA_PUT_U32( msg, NL80211_ATTR_IFINDEX, ifindex);

    send_and_wait4response(msg);
}

prefix_ senf::emu::WirelessNLController::IfaceNameSet senf::emu::WirelessNLController::all_interfaces()
    const
{
    IfaceNameSet ifaceNames;
    // search for all wireless interfaces with our phy index
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr ("/sys/class/net"); itr != end_itr; ++itr ) {
        // hmpf.
#       if BOOST_FILESYSTEM_VERSION == 3
            std::string ifname (itr->path().filename().string());
#       else
            std::string ifname (itr->path().filename());
#       endif
        if (if_nameto_phy_index(ifname) == phyIndex_)
            ifaceNames.insert( ifname);
    }
    return ifaceNames;
}

prefix_ unsigned senf::emu::WirelessNLController::distanceToCoverageClass(unsigned distance)
{
    return std::min( ((distance + 449) / 450), 255u);
}

prefix_ void senf::emu::WirelessNLController::coverageClass(unsigned coverage)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_SET_WIPHY, CIB_PHY, NLM_F_ACK));

    NLA_PUT_U8( msg, NL80211_ATTR_WIPHY_COVERAGE_CLASS, std::min( coverage, 255u));

    send_and_wait4response(msg);
}

prefix_ unsigned senf::emu::WirelessNLController::coverageClass()
{
    getWiphy();
    return coverageClass_;
}

prefix_ bool senf::emu::WirelessNLController::hasHTCapabilities()
{
    getWiphy();
    return hasHTCapabilities_;
}

prefix_ bool senf::emu::WirelessNLController::hasVHTCapabilities()
{
    getWiphy();
    return hasVHTCapabilities_;
}

prefix_ senf::emu::HTCapabilitiesInfo const & senf::emu::WirelessNLController::htCapabilities()
{
    getWiphy();
    return htCapabilities_;
}

prefix_ senf::emu::VHTCapabilitiesInfo const & senf::emu::WirelessNLController::vhtCapabilities()
{
    getWiphy();
    return vhtCapabilities_;
}

namespace {
    template <typename T>
    T readTokenFromFile(boost::filesystem::path const & path)
    {
        boost::filesystem::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path, std::ios::in);
        T token;
        file >> token;
        file.close();
        return token;
    }

    int readIntFromFile(boost::filesystem::path const & path)
    {
        try {
            return readTokenFromFile<int>(path);
        } catch (std::exception & e) {}
        return -1;
    }

    std::string readStringFromFile(boost::filesystem::path const & path)
    {
        try {
            return readTokenFromFile<std::string>(path);
        } catch (std::exception & e) {}
        return "";
    }

    namespace fs = ::boost::filesystem;

    senf::MACAddress readMACAddressFromFile(fs::path const & path)
    {
        fs::ifstream file;
        file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        file.open(path, std::ios::in);
        senf::MACAddress addr;
        file >> addr;
        file.close();
        return addr;
    }
}

prefix_ int senf::emu::WirelessNLController::if_nameto_phy_index(std::string const & ifname)
{
    return readIntFromFile(
            boost::filesystem::path("/sys/class/net")/ifname/"phy80211/index");
}

prefix_ int senf::emu::WirelessNLController::phy_nametoindex(std::string const & phyname)
{
    return readIntFromFile(
            boost::filesystem::path("/sys/class/ieee80211")/phyname/"index");
}

prefix_ std::string senf::emu::WirelessNLController::if_nameto_phy_name(std::string const & ifname)
{
    return readStringFromFile(
            boost::filesystem::path("/sys/class/net")/ifname/"phy80211/name");
}

prefix_ std::string senf::emu::WirelessNLController::macToPhy(senf::MACAddress const & mac)
{
    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr ("/sys/class/ieee80211/"); itr != end_itr; ++itr) {
        try {
            if (readMACAddressFromFile(itr->path()/"macaddress") == mac)
                return itr->path().filename().string();
        } catch (std::exception &) {}
    }
    return "unknown mac";
}

prefix_ int senf::emu::WirelessNLController::channelWidth(ChannelMode::Enum channelMode)
{
    switch (channelMode) {
    case ChannelMode::NoHT20:
        return NL80211_CHAN_WIDTH_20_NOHT;
    case ChannelMode::HT20:
        return NL80211_CHAN_WIDTH_20;
    case ChannelMode::HT40Minus:
    case ChannelMode::HT40Plus:
        return NL80211_CHAN_WIDTH_40;
    case ChannelMode::VHT80:
        return NL80211_CHAN_WIDTH_80;
    }
    throw InvalidArgumentException("invalid channelMode ") << channelMode;
}

prefix_ int senf::emu::WirelessNLController::channelType(ChannelMode::Enum channelMode)
{
    switch (channelMode) {
    case ChannelMode::NoHT20:
        return NL80211_CHAN_NO_HT;
    case ChannelMode::HT20:
        return NL80211_CHAN_HT20;
    case ChannelMode::HT40Minus:
        return NL80211_CHAN_HT40MINUS;
    case ChannelMode::HT40Plus:
        return NL80211_CHAN_HT40PLUS;
    case ChannelMode::VHT80:
        return -1;
    }
    throw InvalidArgumentException("invalid channelMode ") << channelMode;
}

prefix_ senf::emu::WirelessNLController::frequency_type senf::emu::WirelessNLController::centerFreq(
        frequency_type freq, ChannelMode::Enum channelMode)
{
    unsigned int vht80[] = {
        MHZ_TO_KHZ(4920), MHZ_TO_KHZ(5180), MHZ_TO_KHZ(5260), MHZ_TO_KHZ(5500),
        MHZ_TO_KHZ(5580), MHZ_TO_KHZ(5660), MHZ_TO_KHZ(5745) };
    unsigned j;

    switch (channelMode) {
    case ChannelMode::NoHT20:
    case ChannelMode::HT20:
        return freq;
    case ChannelMode::HT40Minus:
        return freq - MHZ_TO_KHZ(10);
    case ChannelMode::HT40Plus:
        return freq + MHZ_TO_KHZ(10);
    case ChannelMode::VHT80:
        for (j = 0; j < ARRAY_SIZE(vht80); j++) {
            if (freq >= vht80[j] && freq < vht80[j] + MHZ_TO_KHZ(80))
                break;
        }
        if (j == ARRAY_SIZE(vht80))
            throw InvalidArgumentException("invalid frequency ") << freq;
        return vht80[j] + MHZ_TO_KHZ(30);
    }
    throw InvalidArgumentException("invalid channelMode ") << channelMode;
}


prefix_ void senf::emu::WirelessNLController::mesh_leave()
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_LEAVE_MESH, CIB_IF, NLM_F_ACK));
    send_and_wait4response(msg);
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr senf::emu::WirelessNLController::mesh_join(
        std::string const & meshId, frequency_type freq, ChannelMode::Enum channelMode)
{
    return MeshJoinParameters::ptr( new MeshJoinParameters(
            membind(&WirelessNLController::do_mesh_join, this), meshId, freq, channelMode) );
}

prefix_ void senf::emu::WirelessNLController::do_mesh_join(MeshJoinParameters const & parameters)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_JOIN_MESH, CIB_IF, NLM_F_ACK));

    NLA_PUT( msg, NL80211_ATTR_MESH_ID, parameters.meshId_.length(), parameters.meshId_.c_str());

    nlPutChannelDef(msg, parameters.freq_, parameters.channelMode_);

    if (parameters.beaconInterval_)
        NLA_PUT_U32( msg, NL80211_ATTR_BEACON_INTERVAL, parameters.beaconInterval_.get());

    {
        nl_nested_attr_ptr msgAttr (msg, NL80211_ATTR_MESH_SETUP);
        if (! boost::empty(parameters.ies_))
            NLA_PUT( msg, NL80211_MESH_SETUP_IE, boost::size(parameters.ies_), &(*boost::begin(parameters.ies_)));
        if (parameters.vendorMetric_)
            NLA_PUT_U8( msg, NL80211_MESH_SETUP_ENABLE_VENDOR_METRIC, parameters.vendorMetric_.get());
        if (parameters.vendorPathSelection_)
            NLA_PUT_U8( msg, NL80211_MESH_SETUP_ENABLE_VENDOR_PATH_SEL, parameters.vendorPathSelection_.get());
        if (parameters.vendorSynchronization_)
            NLA_PUT_U8( msg, NL80211_MESH_SETUP_ENABLE_VENDOR_SYNC, parameters.vendorSynchronization_.get());
    }

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::ibss_leave()
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_LEAVE_IBSS, CIB_IF, NLM_F_ACK));
    send_and_wait4response(msg);
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr senf::emu::WirelessNLController::ibss_join(
        std::string const & ssid, frequency_type freq, ChannelMode::Enum channelMode)
{
    return IbssJoinParameters::ptr( new IbssJoinParameters(
            membind(&WirelessNLController::do_ibss_join, this), ssid, freq, channelMode) );
}

prefix_ void senf::emu::WirelessNLController::do_ibss_join(IbssJoinParameters const & parameters)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_JOIN_IBSS, CIB_IF, NLM_F_ACK));

    NLA_PUT( msg, NL80211_ATTR_SSID, parameters.ssid_.length(), parameters.ssid_.c_str());

    nlPutChannelDef(msg, parameters.freq_, parameters.channelMode_);

    NLA_PUT_FLAG( msg, NL80211_ATTR_FREQ_FIXED);

    if (parameters.handleDFS_)
        NLA_PUT_FLAG(msg, NL80211_ATTR_HANDLE_DFS);

    if (! boost::empty(parameters.ies_))
        NLA_PUT( msg, NL80211_ATTR_IE, boost::size(parameters.ies_), &(*boost::begin(parameters.ies_)));

    if (parameters.bssid_)
        NLA_PUT( msg, NL80211_ATTR_MAC, 6, &parameters.bssid_);

    if (parameters.beaconInterval_)
        NLA_PUT_U32( msg, NL80211_ATTR_BEACON_INTERVAL, parameters.beaconInterval_.get());

    if (parameters.htCapabilities_) {
        NLA_PUT( msg, NL80211_ATTR_HT_CAPABILITY,
                 sizeof(parameters.htCapabilities_.get()),
                 parameters.htCapabilities_.get_ptr());
        NLA_PUT( msg, NL80211_ATTR_HT_CAPABILITY_MASK,
                 sizeof(parameters.htCapabilitiesMask_.get()),
                 parameters.htCapabilitiesMask_.get_ptr());
    }

    send_and_wait4response(msg);
}

prefix_ void senf::emu::WirelessNLController::do_trigger_scan(std::vector<frequency_type> const & frequencies)
{
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_TRIGGER_SCAN, CIB_IF, NLM_F_ACK));

    nl_msg_ptr ssids (nlMsg());
    NLA_PUT(ssids, 1, 0, "");
    NLA_PUT_NESTED( msg, NL80211_ATTR_SCAN_SSIDS, ssids);

    if (not frequencies.empty()) {
        nl_msg_ptr freqs (nlMsg());
        int i = 1;
        for (frequency_type f : frequencies)
            NLA_PUT_U32(freqs, i++, KHZ_TO_MHZ(f));
        NLA_PUT_NESTED( msg, NL80211_ATTR_SCAN_FREQUENCIES, freqs);
    }

    send_and_wait4response(msg, &WirelessNLController::triggerScan_cb);
}

prefix_ int senf::emu::WirelessNLController::triggerScan_cb(nl_msg * msg)
{
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];
    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    switch (msgHdr->cmd) {
    case NL80211_CMD_SCAN_ABORTED:
        return processScanResponse(msgHdr->cmd, msgAttr);
    case NL80211_CMD_NEW_SCAN_RESULTS:
        return processScanResponse(msgHdr->cmd, msgAttr);
    }

    return NL_SKIP;
}

prefix_ std::set<senf::emu::WirelessNLController::ScanResults> const & senf::emu::WirelessNLController::getScan()
{
    scanResults_.clear();
    nl_msg_ptr msg (nlMsgHeader( NL80211_CMD_GET_SCAN, CIB_IF, NLM_F_DUMP));
    send_and_wait4response(msg, &WirelessNLController::getScan_cb);
    return scanResults_;
}

prefix_ int senf::emu::WirelessNLController::getScan_cb(nl_msg * msg)
{
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];
    nlattr * bss[NL80211_BSS_MAX + 1];
    
    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);
    
    if (not msgAttr[NL80211_ATTR_BSS])
        return NL_SKIP;

    if (nla_parse_nested(bss, NL80211_BSS_MAX, msgAttr[NL80211_ATTR_BSS],
                         NLAttributePolicies::bssPolicy) != 0)
        return NL_SKIP;

    if (!bss[NL80211_BSS_BSSID])
        return NL_SKIP;

    ScanResults res;
    res.bssId = senf::MACAddress::from_data((char*)nla_data(bss[NL80211_BSS_BSSID]));
    if (bss[NL80211_BSS_TSF])
        res.tsf = nla_get_u64(bss[NL80211_BSS_TSF]);
    if (bss[NL80211_BSS_FREQUENCY])
        res.frequency = MHZ_TO_KHZ(nla_get_u32(bss[NL80211_BSS_FREQUENCY]));
    if (bss[NL80211_BSS_BEACON_INTERVAL])
        res.beaconInterval = nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]);
    if (bss[NL80211_BSS_CAPABILITY])
        res.capability = nla_get_u16(bss[NL80211_BSS_CAPABILITY]);
    if (bss[NL80211_BSS_SIGNAL_MBM])
        res.signalMBM = nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]);
    if (bss[NL80211_BSS_SIGNAL_UNSPEC])
        res.signalUnspec = nla_get_u8(bss[NL80211_BSS_SIGNAL_UNSPEC]);
    scanResults_.insert(res);
    
    return NL_SKIP;
}

prefix_ int senf::emu::WirelessNLController::processScanResponse(std::uint8_t cmd, nlattr ** msgAttr)
{
    ScanEvent event;
    if (cmd == NL80211_CMD_SCAN_ABORTED) {
        event.type = ScanEvent::Type::ABORTED;
    }
    else if (cmd == NL80211_CMD_NEW_SCAN_RESULTS) {
        event.type = ScanEvent::Type::COMPLETED;
    }
    else {
        return NL_SKIP;
    }

    scanEvent.signal(event);

    return NL_OK;
}

prefix_ void senf::emu::WirelessNLController::get_multicastGroups()
{
    int ctrlid = genl_ctrl_resolve(nl_sock_.get(), "nlctrl");
    if (ctrlid < 0)
        SENF_THROW_SYSTEM_EXCEPTION( "nlctrl not found");

    nl_msg_ptr msg (nlMsg());
    genlmsg_put( msg.get(),
            NL_AUTO_PID, NL_AUTO_SEQ,
            ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);

    NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, "nl80211");

    send_and_wait4response(msg, &WirelessNLController::getMCGroups_cb);
}

prefix_ int senf::emu::WirelessNLController::getMCGroups_cb(nl_msg * msg)
{
    nlattr * msgAttr[CTRL_ATTR_MAX + 1];

    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, CTRL_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    if (not msgAttr[CTRL_ATTR_MCAST_GROUPS])
        return NL_SKIP;

    multicastGroups_.clear();
    FOREACH_NESTED_ATTR( mcgrp, msgAttr[CTRL_ATTR_MCAST_GROUPS] ) {
        nlattr * grpAttr[CTRL_ATTR_MCAST_GRP_MAX + 1];
        nla_parse(grpAttr, CTRL_ATTR_MCAST_GRP_MAX, nla_attr_data(mcgrp), nla_len(mcgrp), NULL);

        if (not grpAttr[CTRL_ATTR_MCAST_GRP_NAME] or not grpAttr[CTRL_ATTR_MCAST_GRP_ID])
            continue;

        std::string grpName (
                reinterpret_cast<const char*>(nla_data(grpAttr[CTRL_ATTR_MCAST_GRP_NAME])),
                nla_len(grpAttr[CTRL_ATTR_MCAST_GRP_NAME])-1);
        int grpId (nla_get_u32(grpAttr[CTRL_ATTR_MCAST_GRP_ID]));
        multicastGroups_[grpName] = grpId;
    }

    return NL_OK;
}

prefix_ void senf::emu::WirelessNLController::processReadEvent(int eventMask)
{
    nl_recvmsgs(nl_event_sock_.get(), nl_event_cb_.get());
}

prefix_ int senf::emu::WirelessNLController::netlink_event_cb(nl_msg * msg)
{
    nlattr * msgAttr[NL80211_ATTR_MAX + 1];
    genlmsghdr * msgHdr = reinterpret_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
    nla_parse(msgAttr, NL80211_ATTR_MAX, genlmsg_attrdata(msgHdr, 0), genlmsg_attrlen(msgHdr, 0), NULL);

    if (phyIndex_ != -1 and msgAttr[NL80211_ATTR_WIPHY]) {
        if (phyIndex_ != int(nla_get_u32(msgAttr[NL80211_ATTR_WIPHY])))
            return NL_SKIP;
    }
    if (ifIndex_ != 0 and msgAttr[NL80211_ATTR_IFINDEX]) {
        if (ifIndex_ != int(nla_get_u32(msgAttr[NL80211_ATTR_IFINDEX])))
            return NL_SKIP;
    }

    switch (msgHdr->cmd) {
    case NL80211_CMD_RADAR_DETECT:
        return processRadarEvent(msgAttr);
    case NL80211_CMD_REG_CHANGE:
        return processRegDomainEvent(msgAttr);
    }

    return NL_OK;
}

prefix_ int senf::emu::WirelessNLController::processRadarEvent(nlattr ** msgAttr)
{
    if (not msgAttr[NL80211_ATTR_RADAR_EVENT] or not msgAttr[NL80211_ATTR_WIPHY_FREQ])
        return NL_SKIP;

    RadarEvent event;
    event.type = RadarEvent::Type(nla_get_u32(msgAttr[NL80211_ATTR_RADAR_EVENT]));
    event.frequency = nla_get_u32(msgAttr[NL80211_ATTR_WIPHY_FREQ]);

    radarEvent.signal(event);

    return NL_OK;
}

prefix_ int senf::emu::WirelessNLController::processRegDomainEvent(nlattr ** msgAttr)
{
    if (not msgAttr[NL80211_ATTR_REG_TYPE])
        return NL_SKIP;

    RegulatoryDomainChangeEvent event;
    event.type = RegulatoryDomainChangeEvent::Type(nla_get_u8(msgAttr[NL80211_ATTR_REG_TYPE]));

    regulatoryDomainChangeEvent.signal(event);

    return NL_OK;
}

prefix_ void senf::emu::WirelessNLController::initEventHandling()
{
    if (nl_event_sock_)
        return;
    initNlSock(nl_event_sock_);
    initNlCb(nl_event_sock_, nl_event_cb_, netlinkEventCb_);
    nlEvent_.reset( new scheduler::FdEvent(
            "WirelessNLControllerEvent", senf::membind(&WirelessNLController::processReadEvent, this),
            nl_socket_get_fd(nl_event_sock_.get()), senf::scheduler::FdEvent::EV_READ));
}

prefix_ void senf::emu::WirelessNLController::deinitEventHandling()
{
    nlEvent_.reset();
    nl_event_cb_.release();
    nl_event_sock_.release();
}

prefix_ int senf::emu::WirelessNLController::multicastGroupId(NetlinkMulticastGroup::Enum group)
{
    get_multicastGroups();

    std::string groupName;
    switch (group) {
    case NetlinkMulticastGroup::Config:
        groupName = "config";
        break;
    case NetlinkMulticastGroup::Scan:
        groupName = "scan";
        break;
    case NetlinkMulticastGroup::Regulatory:
        groupName = "regulatory";
        break;
    case NetlinkMulticastGroup::Mlme:
        groupName = "mlme";
        break;
    case NetlinkMulticastGroup::Vendor:
        groupName = "vendor";
        break;
    }
    if (not senf::contains(multicastGroups_, groupName))
        throw InvalidArgumentException("invalid group ") << groupName;
    return multicastGroups_[groupName];
}

prefix_ void senf::emu::WirelessNLController::join_multicastGroup(NetlinkMulticastGroup::Enum group)
{
    if (senf::contains(joinedMCGroups_, group))
        return;

    if (joinedMCGroups_.empty())
        initEventHandling();

    int r = nl_socket_add_membership(nl_event_sock_.get(), multicastGroupId(group));
    if (r) {
        if (joinedMCGroups_.empty())
            deinitEventHandling();
        throw NetlinkException(r);
    }

    joinedMCGroups_.insert(group);
}

prefix_ void senf::emu::WirelessNLController::leave_multicastGroup(NetlinkMulticastGroup::Enum group)
{
    if (not senf::contains(joinedMCGroups_, group))
        return;

    int r = nl_socket_drop_memberships(nl_event_sock_.get(), multicastGroupId(group));
    if (r)
        throw NetlinkException(r);

    joinedMCGroups_.erase(group);

    if (joinedMCGroups_.empty())
        deinitEventHandling();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// WirelessNLController::IbssJoinParameters

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::IbssJoinParameters(
        Callback cb, std::string const & ssid, frequency_type freq, ChannelMode::Enum channelMode)
    : callback_(cb), handleDFS_(false), ssid_(ssid), freq_(freq), channelMode_(channelMode)
{}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::~IbssJoinParameters()
{
    callback_(*this);
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr senf::emu::WirelessNLController::IbssJoinParameters::handleDFS(bool flag)
{
    handleDFS_ = flag;
    return shared_from_this();
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr senf::emu::WirelessNLController::IbssJoinParameters::bssid(senf::MACAddress const & mac)
{
    bssid_ = mac;
    return shared_from_this();
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr senf::emu::WirelessNLController::IbssJoinParameters::beaconInterval(boost::optional<boost::uint32_t> interval)
{
    beaconInterval_ = interval;
    return shared_from_this();
}

prefix_ void senf::emu::WirelessNLController::IbssJoinParameters::initHTCapabilities()
{
    if (htCapabilities_)
        return;
    htCapabilities_ = ieee80211_ht_cap();
    htCapabilitiesMask_ = ieee80211_ht_cap();
    memset(htCapabilities_.get_ptr(), 0, sizeof(htCapabilities_.get()));
    memset(htCapabilitiesMask_.get_ptr(), 0, sizeof(htCapabilitiesMask_.get()));
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr senf::emu::WirelessNLController::IbssJoinParameters::ampduFactor(unsigned factor)
{
    if (factor > 3)
        throw InvalidArgumentException("invalid ampdu-factor: ") << factor;
    initHTCapabilities();
    htCapabilities_->ampdu_params_info |= factor & 0x3;
    htCapabilitiesMask_->ampdu_params_info |= 0x3; /* 2 bits for factor */
    return shared_from_this();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// WirelessNLController::MeshJoinParameters

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::MeshJoinParameters(
        Callback cb, std::string const & meshId, frequency_type freq, ChannelMode::Enum channelMode)
    : callback_(cb), meshId_(meshId), freq_(freq), channelMode_(channelMode)
{}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::~MeshJoinParameters()
{
    callback_(*this);
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr senf::emu::WirelessNLController::MeshJoinParameters::beaconInterval(boost::optional<boost::uint32_t> interval)
{
    beaconInterval_ = interval;
    return shared_from_this();
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr senf::emu::WirelessNLController::MeshJoinParameters::vendorMetric(bool enable)
{
    vendorMetric_ = enable ? 1 : 0;
    return shared_from_this();
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr senf::emu::WirelessNLController::MeshJoinParameters::vendorPathSelection(bool enable)
{
    vendorPathSelection_ = enable ? 1 : 0;
    return shared_from_this();
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr senf::emu::WirelessNLController::MeshJoinParameters::vendorSynchronization(bool enable)
{
    vendorSynchronization_ = enable ? 1 : 0;
    return shared_from_this();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// NetlinkException

prefix_ senf::emu::NetlinkException::NetlinkException(int code, std::string const & descr)
    : code_(code)
{
    (*this) << "Netlink exception (" << senf::str(errorNumber()) << ") [" << errorString() << "] " << descr;
}

prefix_ int senf::emu::NetlinkException::errorNumber()
    const
{
    return code_;
}

prefix_ char const * senf::emu::NetlinkException::errorString()
    const
{
    return nl_geterror(code_);
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
