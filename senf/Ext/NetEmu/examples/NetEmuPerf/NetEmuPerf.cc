// $Id$
//
// Copyright (C) 2013

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

    class Sender : public module::Module
    {
        SENF_PPI_MODULE(Sender);
    public:
        connector::ActiveOutput<senf::EthernetPacket> output;
        
        Sender(senf::MACAddress const & src, senf::MACAddress const & dst)
            : src_(src), dst_ (dst)
        {
            noroute(output);
        }
        
    private:
        void onTimer()
        {
        }
        
        senf::MACAddress src_;
        senf::MACAddress dst_;
    };

    class Receiver : public module::Module
    {
        SENF_PPI_MODULE(Receiver);
    public:
        connector::PassiveInput<senf::EthernetPacket> input;
        
        Receiver(senf::MACAddress const & src, senf::MACAddress const & dst)
            : src_ (src), dst_(dst)
        {
            noroute(input);
            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest(&Receiver::request);
        }
        
    private:
        void request()
        {
            senf::EthernetPacket const & eth (input());
        }
        
        senf::MACAddress src_;
        senf::MACAddress dst_;
    };
    
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
    
    template <class Interface>
    class App : public module::Module
    {
        SENF_PPI_MODULE(App);
    public:
        typedef typename OurDecoration::template apply<Interface>::type DecoInterface;
        DecoInterface iface;
        
        senf::ClockService::clock_type startTime;
        senf::ClockService::clock_type period;
        senf::ClockService::clock_type nextTime;
        senf::scheduler::SignalEvent sigint;
        senf::scheduler::SignalEvent sigterm;
        
        App(std::string const & iface_, unsigned freq, unsigned burst, unsigned qlen, unsigned txbuf)
            : iface (iface_),
              startTime(senf::scheduler::now()),
              period(senf::ClockService::seconds(1)),
              nextTime(startTime + period),
              sigint (SIGINT, boost::bind(&App::terminate, this)),
              sigterm (SIGTERM, boost::bind(&App::terminate, this))
        {
            iface.interface().maxBurst(burst);
            iface.interface().qlen(qlen);
            iface.interface().sndBuf(txbuf);
            
            iface.interface().enable();
            
            WLANInterface *wif (dynamic_cast<WLANInterface*>(&iface.interface()));
            if (wif) {
                wif->join(freq, 40, 4711u);
            }
        }
        
        virtual void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) = 0;
        
        void statsTimer()
        {
            v_stats(nextTime - startTime, period);
            nextTime += period;
        };
        
        void terminate()
        {
            std::cerr << "bye." << std::endl;
            senf::scheduler::terminate();
        }
    };
    
    template <class Interface>
    class AppRx : public App<Interface>
    {
    public:
        Receiver receiver;
        
        AppRx(std::string const & iface_, unsigned freq, senf::MACAddress peer, unsigned burst, unsigned qlen, unsigned txbuf)
            : App<Interface> (iface_, freq, burst, qlen, txbuf),
            receiver(peer, App<Interface>::iface.interface().id())
        {
            ppi::connect(App<Interface>::iface, receiver);            
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << " queue-drops " << App<Interface>::iface.interface().rxQueueDropped();
            std::cout << std::endl;
        }
    };
    
    template <class Interface>
    class AppTx : public App<Interface>
    {
    public:
        Sender sender;
        
        AppTx(std::string const & iface_, unsigned freq, senf::MACAddress peer, unsigned burst, unsigned qlen, unsigned txbuf)
            : App<Interface> (iface_, freq, burst, qlen, txbuf),
            sender(App<Interface>::iface.interface().id(), peer)
        {
            ppi::connect(sender, App<Interface>::iface);            
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << std::endl;
        }
    };
    
    template <class Application>
    void run(std::string iface, unsigned freq, senf::MACAddress peer, 
             unsigned burst, unsigned qlen, unsigned txbuf)
    {
        Application app (iface, freq, peer, burst, qlen, txbuf);
        ppi::run();
    }    
}

int main(int argc, char ** argv)
{
    if (argc < 7) {
        std::cerr << "Usage:\n    " << argv[0] << "\n"
                  << "        <iface> <freq> <peer>\n"
                  << "        <burst> <qlen> <txbuf> [[NO]RT] [[NO]WLAN]\n"
                  << std::endl;
        return 1;
    }
    
    std::string iface (argv[1]);
    unsigned freq (boost::lexical_cast<unsigned>(argv[2]));
    senf::MACAddress peer (senf::MACAddress::from_string(argv[3]));
    
    unsigned burst (boost::lexical_cast<unsigned>(argv[4]));
    unsigned qlen (boost::lexical_cast<unsigned>(argv[5]));
    unsigned txbuf (boost::lexical_cast<unsigned>(argv[6]));
    
    bool enableRT (true);
    bool useWlan (false);
    
    for (int i (8); i < argc; ++ i) {
        bool state (true);
        char * arg (argv[i]);
        if (*arg == 'N') {
            state = false;
            if (*(++ arg) == 'O')
                ++ arg;
        }
        switch (*arg) {
        case 'R': enableRT = state; break;
        case 'W': useWlan = state; break;
        default: std::cerr << "invalid flag " << argv[i] << std::endl; ::exit(1);
        }
    }
    
    if (enableRT) {
        sched_param schedpm;
        schedpm.sched_priority = 1;
        if (::sched_setscheduler(::getpid(), SCHED_FIFO, &schedpm) >= 0)
            std::cerr << "SCHED_FIFO realtime scheduling enabled" << std::endl;
    }
    
    senf::scheduler::watchdogTimeout(0); // allow debugging using gdb
    
    senf::console::Server::start(senf::INet4SocketAddress(senf::INet4Address::None, 23232u));
    
    if (useWlan) 
        run<AppRx<WLANInterface> >(iface, freq, peer,
                                   burst, qlen, txbuf);
    else
        run<AppRx<EthernetInterface> >(iface, freq, peer, 
                                       burst, qlen, txbuf);
    
    return 0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "main.mpp"


// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// End:
