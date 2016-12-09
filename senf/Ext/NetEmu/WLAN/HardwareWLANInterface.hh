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

        std::string const & device() const;
                                        ///< Get name of physical device
        std::string const & monitorDevice() const;
                                        ///< Get current monitor %interface

        void setREDFilterCallbackRx(senf::ppi::REDFilterCallback const & cb);
        void setREDFilterCallbackTx(senf::ppi::REDFilterCallback const & cb);

        MonitorDataFilterStatistics filterStats();
        void monitorDropUnknownMCS(bool q);

        WirelessNLController::IbssJoinParameters::ptr joinAdhoc(std::string const & ssid, unsigned int freq, unsigned int bandwidth);
        WirelessNLController::MeshJoinParameters::ptr joinMesh(std::string const & meshId, unsigned int freq, unsigned int bandwidth);
        void leaveCell();

        int frequencyOffset() const;
        void frequencyOffset(int offset);

        void restrictBand(WirelessNLController::Band_t band);
        void restrictBand();

        void refreshProperties();

        struct HTMode {
            enum Enum { Disabled, Enabled, HT20only, HT40only };
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
        std::pair<unsigned,unsigned> txDropped();

        unsigned maxBurst() const;
        void maxBurst(unsigned maxBurst);

        WirelessNLController::DFSState::Enum dfsState(unsigned freq, unsigned bw);

        void dumpMmapStats(std::ostream & os);
        void dumpFilterStats(std::ostream & os);

        WifiStatisticsMap const & statisticsMap(std::uint32_t tag);

        void frequencyHint();

        void spectralScanCallback(AthSpectralScan::AthSpectralScanCallback const & cb);
        void spectralScanCallback();
        void spectralScanStats(std::ostream & os);
        
    protected:
        NetdeviceController netctl_;
        mutable WirelessNLController wnlc_;

    private:
        void registerFrequencies();
        void registerModulations();
        void registerTxPowers();

        void init();
        void init_sockets();
        void close_sockets();

        void openMonitorSocket();
        void closeMonitorSocket();
        void openDataSocket();
        void closeDataSocket();
        
        // interface
        virtual void v_enable();
        virtual void v_disable();
        virtual bool v_enabled() const;
        virtual void v_id(MACAddress const & id);
        virtual MACAddress v_id() const;
        virtual bool v_annotationMode() const;
        virtual void v_annotationMode(bool a);
        virtual bool v_promisc() const;
        virtual void v_promisc(bool v);
        virtual void v_mcAdd(MACAddress const & address);
        virtual void v_mcDrop(MACAddress const & address);
        virtual unsigned v_mtu() const;
        virtual void v_mtu(unsigned v);
        virtual void v_flushRxQueues();
        virtual void v_flushTxQueues();
        virtual void v_coverageRange(unsigned distance);

        void do_ibss_join(WirelessNLController::IbssJoinParameters const & parameters);
        void do_mesh_join(WirelessNLController::MeshJoinParameters const & parameters);

        // modulation
        virtual ModulationParameter::id_t v_modulationId() const;
        virtual void v_modulationId(ModulationParameter::id_t);

        // channel frequency
        virtual unsigned v_frequency() const;
        virtual unsigned v_bandwidth() const;
        virtual void v_frequency(unsigned freq, unsigned bw);

        // txpower
        virtual int v_txPower() const;
        virtual void v_txPower(int power);

        std::string dev_;
        std::string monitorDev_;
        AthSpectralScan spectralScanner_;
        WifiStatistics wifiStatistics_;
        bool promisc_; // cache
        int frequencyOffset_;
        int restrictedBand_;
        HTMode::Enum htMode_;
        ModulationParameter::id_t modId_;
        unsigned bw_;
        int txPower_;
        unsigned rcvBufSize_;
        unsigned sndBufSize_;
        unsigned qlen_;

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
