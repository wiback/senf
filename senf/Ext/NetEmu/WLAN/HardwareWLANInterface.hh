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
    \brief HardwareWLANInterface public header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_HardwareWLANInterface_
#define HH_SENF_Ext_NetEmu_WLAN_HardwareWLANInterface_ 1

// Custom includes
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Utils/Logger/Log.hh>
#include "WLANInterface.hh"
#include "WirelessNLController.hh"
#include "AthSpectralScan.hh"
#include "WifiStatistics.hh"

#include "HardwareWLANInterface.ih"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Hardware WLAN Interface

        This class adapts a real, hardware based WLAN interface into the \ref senf_emu_interfaces.

        \see WirelessInterface
        \ingroup senf_emu_interface_implementations
     */
    class HardwareWLANInterface
        : private detail::HardwareWLANInterfaceNet,
          public WLANInterface,
          public HardwareInterface
    {
        SENF_LOG_CLASS_AREA();

    public:
        typedef TypedInterfaceDecorator<
            HardwareWLANInterface, WirelessInterface::Decorator> Decorator;

        explicit HardwareWLANInterface(std::pair<std::string,std::string> interfaces);

        virtual ~HardwareWLANInterface();
        
        std::string const & v_device() const override;
                                        ///< Get name of physical device
        std::string const & monitorDevice() const;
                                        ///< Get current monitor %interface

        void setREDFilterCallbackRx(senf::ppi::REDFilterCallback const & cb);
        void setREDFilterCallbackTx(senf::ppi::REDFilterCallback const & cb);
        void setREDFilterCallbackRxMonitor(senf::ppi::REDFilterCallback const & cb);

        MonitorDataFilterStatistics filterStats();
        void monitorDropUnknownMCS(bool q);

        WirelessNLController::JoinParameters::ptr joinAdhoc(std::string const & ssid, unsigned int freq, unsigned int bandwidth, bool & success);
        WirelessNLController::JoinParameters::ptr joinMesh(std::string const & meshId, unsigned int freq, unsigned int bandwidth, bool & success);
        void leaveCell();
        bool cellJoined() const;
        void setCellJoined(unsigned bandwidth);  // bw==0 => cell left (close data socket)
        
        int frequencyOffset() const;
        void frequencyOffset(int offset);

        void restrictBand(WirelessNLController::Band_t band);
        void restrictBand();

        void refreshProperties();

        struct HTMode {
            enum Enum { Disabled, Enabled, HT20only, HT40only, VHT80only };
        };
        void htMode(HTMode::Enum mode);
        HTMode::Enum htMode() const;

        unsigned sndBuf();
        void     sndBuf(unsigned sndbuf);
        unsigned rcvBuf();
        void     rcvBuf(unsigned rcvbuf);

        // Changing these parameters while the interface is enabled
        // will temporarily disable the interface !!

        unsigned qlen() const;
        void     qlen(unsigned qlen);

        std::pair<unsigned,unsigned> rxDropped();
        std::tuple<unsigned,unsigned,unsigned> txDropped();

        unsigned maxBurst() const;
        void maxBurst(unsigned maxBurst);

        VLanId const & pvid() const;
        bool pvid(VLanId const & p, bool accessMode); 
        std::uint32_t vlanMismatchRx();
        std::uint32_t vlanMismatchTx();
        
        std::pair<senf::emu::WirelessNLController::DFSState::Enum,std::uint32_t> dfsState(unsigned freq, unsigned bw);
        void startCAC(unsigned int freq, unsigned int bandwidth);
        void cacAborted();

        void dumpMmapStats(std::ostream & os);
        void dumpFilterStats(std::ostream & os);
        bool isDead() const;

        void airTime(bool a);
        WifiStatisticsMap const & statisticsMap(std::uint32_t tag, senf::ClockService::clock_type const & maxAge);
        void dumpWifiStatistics(std::ostream & os);

        void frequencyHint(unsigned bandwidth);

        void spectralScanCallback(AthSpectralScan::AthSpectralScanCallback const & cb);
        void spectralScanCallback();
        void spectralScanStats(std::ostream & os);

        senf::emu::WirelessNLController::NetlinkEvent<RadarEvent> & radarEvent();
        senf::emu::WirelessNLController::NetlinkEvent<ScanEvent>  & scanEvent();

        void triggerScan(std::ostream & os, std::set<WirelessNLController::frequency_type> const & frequencies);
        void getScan(std::ostream & os);
        void dumpSurvey(std::ostream & os);

        WirelessNLController & wirelessNLController();

        // temporary member to allow testing MCS ranges
        void modulationSet(std::set<ModulationParameter::id_t> const & ids = {});

    protected:
        NetdeviceController netctl_;
        mutable WirelessNLController wnlc_;

    private:
        void registerFrequencies();
        void registerModulations();
        void registerModulations_vht(BitrateParameters::VHT_MCSBitmapTable const & vht_mcs_table);
        void registerVHTModulation(unsigned vhtMcsIndex, unsigned nss, unsigned bw, bool shortGI);
        void registerTxPowers();

        void init();
        void init_sockets();
        void close_sockets();

        void openMonitorSocket();
        void closeMonitorSocket();
        void openDataSocket();
        void closeDataSocket();
        
        // interface
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual bool v_enabled() const override;
        virtual void v_id(MACAddress const & id) override;
        virtual MACAddress v_id() const override;
        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool a) override;
        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;
        virtual void v_mcAdd(MACAddress const & address) override;
        virtual void v_mcDrop(MACAddress const & address) override;
        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;
        virtual void v_flushRxQueues() override;
        virtual void v_flushTxQueues() override;
        virtual void v_coverageRange(unsigned distance) override;

        void do_ibss_join(WirelessNLController::JoinParameters const & parameters);
        void do_mesh_join(WirelessNLController::JoinParameters const & parameters);

        // modulation
        virtual ModulationParameter::id_t v_modulationId() const override;
        virtual void v_modulationId(ModulationParameter::id_t) override;
        // channel frequency
        virtual unsigned v_frequency() const override;
        virtual unsigned v_bandwidth() const override;
        virtual void v_frequency(unsigned freq, unsigned bw) override;

        // txpower
        virtual int v_txPower() const override;
        virtual void v_txPower(int power) override;

        std::string dev_;
        std::string monitorDev_;
        AthSpectralScan spectralScanner_;
        WifiStatistics wifiStatistics_;
        int frequencyOffset_;
        int restrictedBand_;
        HTMode::Enum htMode_;
        ModulationParameter::id_t modId_;
        unsigned bw_;
        int txPower_;
        unsigned rcvBufSize_;
        unsigned sndBufSize_;
        unsigned qlen_;
        VLanId  pvid_;
        bool accessMode_;

        friend struct detail::HardwareWLANInterfaceNet;
        friend class WLANMonitorDataFilter;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "HardwareWLANInterface.cci"
//#include "HardwareWLANInterface.ct"
//#include "HardwareWLANInterface.cti"
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
