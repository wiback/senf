//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief main non-inline non-template implementation */

//#include "main.hh"
//#include "main.ih"

// Custom includes
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <senf/PPI.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>
#include <senf/Packets/80211Bundle/InformationElements.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessExtController.hh>
#include <senf/Ext/NetEmu/Ethernet/HardwareEthernetInterface.hh>
#include <senf/Ext/NetEmu/WLAN/HardwareWLANInterface.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Scheduler/SignalEvent.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Utils/Console.hh>

//#include "main.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;
namespace emu = senf::emu;

namespace {
    
    struct WMMParameter
    {
        std::uint8_t  accessCategory;
        std::uint16_t cwMin;
        std::uint16_t cwMax;
        std::uint8_t  aifs;
        std::uint16_t txOp;
    };
    
    // TODO (2014-01-13 mtk): TxOp timings are considered in .11n mode in 3.13 (and hopefully above :)
    static WMMParameter defaultHtWMM[] = {
        // NC
        { NL80211_TXQ_Q_VO,  3,  7,  0,   0 },
        // VO
        { NL80211_TXQ_Q_VI,  3,  7,  2,  10 },
        // VI
        { NL80211_TXQ_Q_BE,  7, 15,  3,  20 },
        // BE
        { NL80211_TXQ_Q_BK,  7, 15,  5,  40 },
    };
    
    class ReceiveGroup : public module::Module
    {
        SENF_PPI_MODULE(ReceiveGroup);
    public:
        connector::PassiveInput<senf::EthernetPacket> input;
        connector::ActiveOutput<senf::EthernetPacket> output;
        senf::MACAddress id_;
        
        ReceiveGroup(senf::emu::Interface const & interface) {
            route( input, output).autoThrottling( false);
            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest( &ReceiveGroup::request);
            id_ = interface.id();
        }
        void request() {
            output(input());
        }
    };
    
    class TransmitGroup : public module::Module
    {
        SENF_PPI_MODULE(TransmitGroup);
    public:
        connector::PassiveInput<senf::Packet> input;
        connector::ActiveOutput<senf::Packet> output;
        
        TransmitGroup(senf::emu::Interface const & interface) {
            route(input, output).autoThrottling( false);
            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest( &TransmitGroup::request);
        }
        void request() { 
            output(input()); 
        }
    };
    
    typedef senf::emu::ApplyInterfaceDecorator<ReceiveGroup, TransmitGroup> OurDecoration;

        class EthernetInterface
        : public emu::HardwareEthernetInterface
    {
    public:
        EthernetInterface(std::string const & iface)
            : emu::HardwareEthernetInterface(iface)
        {}
    };
    
    class WLANInterface
        : public emu::HardwareWLANInterface
    {
    public:
        WLANInterface(std::string const & phy)
            : emu::HardwareWLANInterface(initInterfaces(phy))
        {}
        
        ~WLANInterface()
        {
            try {
                wnlc_.ibss_leave();
            } catch (...) {}
        }
        
        std::pair<std::string,std::string> initInterfaces(std::string const & dev)
        {
            senf::emu::WirelessNLController wnlc(senf::emu::WirelessNLController(dev).phyName());
            // first, remove all existing interfaces of the phy device
            for (std::string iface : wnlc.all_interfaces()) {
                wnlc.del_interface( iface);
            }
            
            // now, add the monitor interface...
            wnlc.add_monInterface(monInterfaceName(wnlc.phyIndex()),
                                  senf::emu::WirelessNLController::MonitorFlags::FCSFail  |
                                  senf::emu::WirelessNLController::MonitorFlags::PLCPFail |
                                  senf::emu::WirelessNLController::MonitorFlags::Control  |
                                  senf::emu::WirelessNLController::MonitorFlags::OtherBSS);
            
            // ... and then the adhoc interface
            std::string ifname (adhocInterfaceName(wnlc.phyIndex()));
            wnlc.add_adhocInterface(ifname);
            
            return std::make_pair(ifname,monInterfaceName(wnlc.phyIndex()));
        }
        
        std::string adhocInterfaceName(int phyIndex)
        {
            return (boost::format("wiback-wlan%d") % phyIndex).str();
        }
        
        std::string monInterfaceName(int phyIndex)
        {
            return (boost::format("wiback-mon%d") % phyIndex).str();
        }
        
        void join(unsigned freq, unsigned bwidth, unsigned netId) 
        {
            std::string ssid;
            senf::MACAddress bssid;
            ssid = "WiBACK-" + std::to_string(netId) + "-" + std::to_string(freq);
            std::uint64_t tmp ((std::uint64_t(netId) << 32) + freq);
            char buf[64];
            sprintf( buf, "00:%x:%x:%x:%x:%x", std::uint32_t((tmp >> 32) & 0xff), std::uint32_t((tmp >> 24) & 0xff),
                     std::uint32_t((tmp >> 16) & 0xff), std::uint32_t((tmp >>  8) & 0xff),
                     std::uint32_t((tmp >>  0) & 0xff));
            bssid = senf::MACAddress::from_string( buf);
            
            joinAdhocNetwork( ssid, freq*1000, bwidth*1000)
                ->bssid( bssid)
                ->beaconInterval(125)
                ->ampduFactor(3);  // WiBACK default. Should be configurable via policy.
                                   // For more recent kernels 3.13++, we should set this to 3 and control the burst length via TxOp
            
            modulation(senf::emu::WLANModulationParameter::MCS, 7);
            txPower(txPowers().back().upper);
            coverageRange(450);
            senf::emu::WirelessNLController wnlc(device());
            // the PID rate controller counts the total number of transmissions not only the retries, hence +1
            wnlc.set_retryLimit(2,2);
            
            // default WiBACK WMM settings 
            for (unsigned i = 0; i < 4; i++){
                wnlc.set_txQueueParameters( defaultHtWMM[i].accessCategory, defaultHtWMM[i].cwMin, defaultHtWMM[i].cwMax, defaultHtWMM[i].aifs, defaultHtWMM[i].txOp);
            }
        }
    };
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "main.mpp"


// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// End:
