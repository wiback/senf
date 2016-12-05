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
    \brief WirelessNLController public header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_WirelessNLController_
#define HH_SENF_Ext_NetEmu_WLAN_WirelessNLController_ 1

// Custom includes
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/optional.hpp>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Utils/Logger/Log.hh>

extern "C" {
#include "nl80211-new.h"
#include "ieee80211.h"
}

#include "Regulatory.hh"

//#include "WirelessNLController.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

// forward declarations:
struct nl_msg;
struct nl_cb;
struct nlattr;
struct nl_sock;
struct nl_cache;
struct genl_family;


namespace senf {
namespace emu {

    class HardwareWLANInterface;
    class WirelessNLController;


    struct HTCapabilitiesInfo
    {
        // Indicates support for receiving LDPC coded packets
        bool rxLDPC;
        // Supported Channel Width
        enum ChannelWidth { HT20, HT20_40=2 };
        ChannelWidth channelWidth;
        // spatial multiplexing power save mode
        enum SMPowerSave { Static, Dynamic, Disabled=3 };
        SMPowerSave smPowerSave;
        // Indicates support for the reception of PPDUs with HT-greenfield format
        bool rxGreenfield;
        // Indicates short GI support for 20 MHz
        bool rxHT20SGI;
        // Indicates short GI support for 40 MHz
        bool rxHT40SGI;
        // Indicates support for the transmission of PPDUs using STBC
        bool txSTBC;
        // Indicates support for the reception of PPDUs using STBC
        enum RXSTBC { NoRxSTBC, rxSTBC_1Stream, rxSTBC_2Stream, rxSTBC_3Stream };
        RXSTBC rxSTBC;
        // Indicates support for HT delayed Block Ack operation
        bool delayedBlockAck;
        // maximum AMSDU length in bytes
        enum MaxAMSDULength { MaxAMSDULength_3839, MaxAMSDULength_7935 };
        MaxAMSDULength maxAMSDULength;
        // DSSS/CCK Mode in 40 MHz
        bool ht40DSSS_CCK;
        // Indicates whether APs receiving this information or reports of this
        // information are required to prohibit 40 MHz transmissions
        bool fortyMHzIntolerant;
        // Indicates support for the LSIG TXOP protection mechanism
        bool lSIG_TXOP_Protection;
    };


    struct NetlinkMulticastGroup
    {
        enum Enum { Config, Scan, Regulatory, Mlme, Vendor };
    };

    struct RadarEvent
    {
        static const NetlinkMulticastGroup::Enum MCGroup = NetlinkMulticastGroup::Mlme;

        enum Type {
            RadarDetected = NL80211_RADAR_DETECTED,
            CACFinished = NL80211_RADAR_CAC_FINISHED,
            CACAborted = NL80211_RADAR_CAC_ABORTED,
            RadarNopFinished = NL80211_RADAR_NOP_FINISHED };

        Type type;
        unsigned frequency;
    };

    struct RegulatoryDomainChangeEvent
    {
        static const NetlinkMulticastGroup::Enum MCGroup = NetlinkMulticastGroup::Regulatory;

        enum Type {
            Country = NL80211_REGDOM_TYPE_COUNTRY,
            World = NL80211_REGDOM_TYPE_WORLD,
            CustomWorld = NL80211_REGDOM_TYPE_CUSTOM_WORLD,
            Intersection = NL80211_REGDOM_TYPE_INTERSECTION };

        Type type;
    };

    struct BitrateParameters
    {
        typedef unsigned short MCSIndex;
        typedef std::set<MCSIndex> MCSIndexSet;
        boost::optional<MCSIndexSet> mcs_24;
        boost::optional<MCSIndexSet> mcs_5;
        typedef uint32_t LegacyBitrate;  // Bitrate in kbps
        typedef std::set<LegacyBitrate> LegacyBitrateSet;
        boost::optional<LegacyBitrateSet> legacy_24;
        boost::optional<LegacyBitrateSet> legacy_5;
    };


    class WirelessNLController
    {
    public:
        SENF_LOG_CLASS_AREA();

    public:
        struct IfaceType {
            enum Enum { Unspecified, AdHoc, Station, AP, APVLan, WDS, Monitor, MeshPoint, Unknown };
            static std::string str(Enum type);
        };
        struct MonitorFlags {
            enum Enum {
                None = 0,                                    // no special flags
                FCSFail =  1 << NL80211_MNTR_FLAG_FCSFAIL,   // pass frames with FCS errors
                PLCPFail = 1 << NL80211_MNTR_FLAG_PLCPFAIL,  // pass frames with bad PLCP
                Control =  1 << NL80211_MNTR_FLAG_CONTROL,   // pass control frames
                OtherBSS = 1 << NL80211_MNTR_FLAG_OTHER_BSS  // show frames from other BSSes
            };
        };
        struct ChannelMode {
            enum Enum {
                NoHT20,
                HT20,
                HT40Minus,
                HT40Plus,
				VHT80,
            };
        };
        struct DFSState {
            enum Enum {
                NoDFS       = -1,
                Usable      = NL80211_DFS_USABLE,
                Unavailable = NL80211_DFS_UNAVAILABLE,
                Available   = NL80211_DFS_AVAILABLE
            };
            static std::string str(Enum state);
        };
        struct TxPowerSetting {
            enum Enum {
                Auto = NL80211_TX_POWER_AUTOMATIC,
                Limit = NL80211_TX_POWER_LIMITED,
                Fixed = NL80211_TX_POWER_FIXED
            };
        };
        enum Band_t {
            BAND_2GHZ = NL80211_BAND_2GHZ,
            BAND_5GHZ = NL80211_BAND_5GHZ
        };

        typedef unsigned bitrate_type;
        typedef unsigned frequency_type;

    private:
        typedef std::multimap<Band_t, frequency_type> Frequencies;
        typedef boost::transform_iterator< ::__gnu_cxx::select2nd<Frequencies::value_type>,
                Frequencies::const_iterator> Frequencies_iterator;
        typedef std::vector<bitrate_type> BitrateList;

        class IbssJoinParameters : boost::noncopyable,
            public boost::enable_shared_from_this<IbssJoinParameters>
        {
            typedef boost::function<void (IbssJoinParameters const &)> Callback;
            typedef boost::shared_ptr<IbssJoinParameters> ptr;

            IbssJoinParameters(Callback cb, std::string const & ssid, frequency_type freq, ChannelMode::Enum channelMode);

            void initHTCapabilities();

            Callback callback_;
            bool handleDFS_;
            std::string ssid_;
            frequency_type freq_;
            ChannelMode::Enum channelMode_;
            senf::MACAddress bssid_;
            std::vector<unsigned char> ies_;
            boost::optional<ieee80211_ht_cap> htCapabilities_;
            boost::optional<ieee80211_ht_cap> htCapabilitiesMask_;
            boost::optional<boost::uint32_t> beaconInterval_;

            friend class WirelessNLController;
            friend class HardwareWLANInterface;

        public:
            ~IbssJoinParameters();

            template <typename ForwardReadableRange>
            IbssJoinParameters::ptr informationElements(ForwardReadableRange const & ies);
            IbssJoinParameters::ptr bssid(senf::MACAddress const & mac);
            IbssJoinParameters::ptr ampduFactor(unsigned factor);
            IbssJoinParameters::ptr beaconInterval(boost::optional<boost::uint32_t> interval);
            IbssJoinParameters::ptr handleDFS(bool flag);
        };

        class MeshJoinParameters : boost::noncopyable,
            public boost::enable_shared_from_this<MeshJoinParameters>
        {
            typedef boost::function<void (MeshJoinParameters const &)> Callback;
            typedef boost::shared_ptr<MeshJoinParameters> ptr;

            MeshJoinParameters(Callback cb, std::string const & meshId, frequency_type freq, ChannelMode::Enum channelMode);

            Callback callback_;
            std::string meshId_;
            frequency_type freq_;
            ChannelMode::Enum channelMode_;
            std::vector<unsigned char> ies_;
            boost::optional<boost::uint32_t> beaconInterval_;
            boost::optional<boost::uint8_t> vendorMetric_;
            boost::optional<boost::uint8_t> vendorPathSelection_;
            boost::optional<boost::uint8_t> vendorSynchronization_;

            friend class WirelessNLController;
            friend class HardwareWLANInterface;

        public:
            ~MeshJoinParameters();

            template <typename ForwardReadableRange>
            MeshJoinParameters::ptr informationElements(ForwardReadableRange const & ies);
            MeshJoinParameters::ptr beaconInterval(boost::optional<boost::uint32_t> interval);
            MeshJoinParameters::ptr vendorMetric(bool enable);
            MeshJoinParameters::ptr vendorPathSelection(bool enable);
            MeshJoinParameters::ptr vendorSynchronization(bool enable);
        };

    public:
        typedef boost::iterator_range<Frequencies_iterator> FrequencyRange;
        typedef boost::iterator_range<BitrateList::iterator> BitrateRange;
        typedef boost::function<int (nl_msg *)> NetlinkMsgCallback;

        WirelessNLController();
        WirelessNLController(std::string const & interface);

        IbssJoinParameters::ptr ibss_join(std::string const & ssid, frequency_type freq, ChannelMode::Enum channelMode);
        void ibss_leave();

        MeshJoinParameters::ptr mesh_join(std::string const & meshId, frequency_type freq, ChannelMode::Enum channelMode);
        void mesh_leave();

        void set_frequency(frequency_type freq, ChannelMode::Enum = ChannelMode::NoHT20);
        void set_txpower(TxPowerSetting::Enum setting, unsigned int mBm);

        void add_monInterface(std::string const & name, int flags = MonitorFlags::None);
        void add_adhocInterface(std::string const & name);
        void add_apInterface(std::string const & name);
        void add_meshInterface(std::string const & name, std::string const & meshId = "");

        void del_interface(std::string const & name);

        void set_retryLimit(boost::uint8_t shortLimit, boost::uint8_t longLimit);

        void set_txQueueParameters(boost::uint8_t queue, boost::uint16_t cwMin, boost::uint16_t cwMax, boost::uint8_t aifs, boost::uint16_t txop);

        DFSState::Enum dfsState(frequency_type freq);
        void start_radarDetection(unsigned int freq, ChannelMode::Enum = ChannelMode::NoHT20);

        void set_regulatory_request(std::string const & alpha2Country);
        void set_regulatory(RegulatoryDomain const & regDomain);
        RegulatoryDomain get_regulatory();

        static unsigned distanceToCoverageClass(unsigned distance);
        void coverageClass(unsigned coverage);
        unsigned coverageClass();

        bool hasHTCapabilities();
        HTCapabilitiesInfo const & htCapabilities();

        typedef std::set<std::string> IfaceNameSet;
        IfaceNameSet all_interfaces() const;

        std::string const & phyName() const;
        int phyIndex() const;

        static int if_nameto_phy_index(std::string const & ifname);
        static std::string if_nameto_phy_name(std::string const & ifname);
        static int phy_nametoindex(std::string const & phyname);
        static std::string macToPhy(senf::MACAddress const & mac);

        static int channelWidth(ChannelMode::Enum channelMode);
        static int channelType(ChannelMode::Enum channelMode);
        static frequency_type centerFreq(frequency_type freq, ChannelMode::Enum channelMode);

        frequency_type frequency();
        FrequencyRange frequencies();
        FrequencyRange frequencies(Band_t band);

        void set_bitrates(BitrateParameters parameters);
        BitrateParameters const & bitrates();  ///< Return valid bitrates

        IfaceType::Enum type();

        std::set<Band_t> const & supportedBands();

        template <typename EventType>
        class NetlinkEvent
        {
        public:
            boost::signals2::signal<void (EventType)> signal;
            void enable();
            void disable();

        private:
            NetlinkEvent(WirelessNLController & wnlc);
            WirelessNLController & wnlc_;
            friend class WirelessNLController;
        };

        NetlinkEvent<RadarEvent> radarEvent;
        NetlinkEvent<RegulatoryDomainChangeEvent> regulatoryDomainChangeEvent;

    private:
        typedef boost::shared_ptr<nl_msg> nl_msg_ptr;
        typedef std::unique_ptr<nl_sock, void(*)(nl_sock *)> nl_sock_ptr;
        typedef std::unique_ptr<nl_cb, void(*)(nl_cb *)> nl_cb_ptr;
        typedef std::unique_ptr<nl_cache, void(*)(nl_cache *)> nl_cache_ptr;
        typedef int (WirelessNLController::* CallbackMemPtr)(nl_msg *);
        enum CmdIdBy { CIB_NONE, CIB_PHY, CIB_IF };

        void init();
        void initNlSock(nl_sock_ptr & sock);
        void initNlCache(nl_sock_ptr & sock, nl_cache_ptr & cache);
        void initNlCb(nl_sock_ptr & sock, nl_cb_ptr & cb, NetlinkMsgCallback & msgcb);

        static nl_msg_ptr nlMsg();
        static void nlPutChannelDef(nl_msg_ptr msg, frequency_type freq, ChannelMode::Enum channelMode);
        nl_msg_ptr nlMsgHeader(uint8_t cmd, CmdIdBy idBy, int flags=0) const;
        void send_and_wait4response(nl_msg_ptr const & msg, CallbackMemPtr cb = nullptr);

        void do_ibss_join(IbssJoinParameters const & parameters);
        void do_mesh_join(MeshJoinParameters const & parameters);

        void getWiphy();

        // netlink callbacks
        int netlink_cb(nl_msg * msg);
        int netlink_event_cb(nl_msg * msg);

        int getWiphy_cb(nl_msg * msg);
        int getSurvey_cb(nl_msg * msg);
        int getIface_cb(nl_msg * msg);
        int getMCGroups_cb(nl_msg * msg);
        int getRegDomain_cb(nl_msg * msg);

        int multicastGroupId(NetlinkMulticastGroup::Enum group);
        void join_multicastGroup(NetlinkMulticastGroup::Enum group);
        void leave_multicastGroup(NetlinkMulticastGroup::Enum group);
        void get_multicastGroups();
        void initEventHandling();
        void deinitEventHandling();
        void processReadEvent(int eventMask);

        int processRadarEvent(nlattr ** msgAttr);
        int processRegDomainEvent(nlattr ** msgAttr);

        Frequencies frequencies_;
        BitrateParameters bitrates_;
        bool firstMutlipartMsg_;
        frequency_type frequency_;
        IfaceType::Enum ifaceType_;
        unsigned coverageClass_;
        bool hasHTCapabilities_;
        HTCapabilitiesInfo htCapabilities_;
        std::set<Band_t> supportedBands_;

        int ifIndex_;
        int phyIndex_;
        std::string phyName_;
        nl_sock_ptr nl_sock_;
        nl_cache_ptr nl_cache_;
        nl_cb_ptr nl_cb_;
        int nl80211Id_;
        CallbackMemPtr callback_;
        NetlinkMsgCallback netlinkMsgCb_;
        NetlinkMsgCallback netlinkEventCb_;
        int bandtype_;
        typedef std::map<frequency_type, DFSState::Enum> DFSStateMap;
        DFSStateMap dfsStates_;
        RegulatoryDomain regDomain_;
        boost::optional<std::set<unsigned short> > tmpMCSRates_;
        boost::optional<std::set<uint32_t> > tmpLegacyRates_;

        nl_sock_ptr nl_event_sock_;
        nl_cb_ptr nl_event_cb_;
        std::set<NetlinkMulticastGroup::Enum> joinedMCGroups_;
        std::map<std::string, int> multicastGroups_;
        boost::scoped_ptr<scheduler::FdEvent> nlEvent_;

        friend class HardwareWLANInterface;
    };


    class NetlinkException : public senf::Exception
    {
    public:
        NetlinkException(int code, std::string const & descr = "");

        int errorNumber() const;
        char const * errorString() const;

    private:
        int code_;
    };
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "WirelessNLController.cci"
#include "WirelessNLController.ct"
//#include "WirelessNLController.cti"
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
