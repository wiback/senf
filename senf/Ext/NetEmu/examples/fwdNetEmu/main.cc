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
            route( input, output); //autoThrottling( false);
            //            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest( &ReceiveGroup::request);

            id_ = interface.id();
        }

        void request() { 
            senf::EthernetPacket const & eth (input());
            senf::MACAddress dst (eth->destination());

            if (dst != id_ || dst.multicast()) {
                if (! eth.annotation<senf::emu::annotations::Quality>().flags.frameDuplicate) {
                    output(eth);
                }
            }
        }
    };

    class TransmitGroup : public module::Module
    {
        SENF_PPI_MODULE(TransmitGroup);
    public:
        connector::PassiveInput<senf::Packet> input;
        connector::ActiveOutput<senf::Packet> output;

        TransmitGroup(senf::emu::Interface const & interface) {
            route( input, output); // autoThrottling( false);
            //            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest( &TransmitGroup::request);
        }

        void request() { 
            senf::Packet const & pkt (input());

            output(pkt); 
        }
    };

    typedef senf::emu::ApplyInterfaceDecorator<ReceiveGroup, TransmitGroup> OurDecoration;


    class Forwarder : public module::Module
    {
        SENF_PPI_MODULE(Forwarder);
    public:
        connector::PassiveInput<senf::EthernetPacket> input;
        connector::ActiveOutput<senf::EthernetPacket> output;

        Forwarder(senf::MACAddress const & src, senf::MACAddress const & dst)
            : first_ (true), src_ (src), dst_ (dst), packetCount_(0), packetIgnored_(0), ttlMin_(255), ttlMax_(0)
        {
            route(input, output);
            input.onRequest(&Forwarder::request);
        }

        unsigned packetCount()
        {
            unsigned res (packetCount_);
            packetCount_ = 0;
            return res;
        }

        unsigned packetIgnored()
        {
            unsigned res (packetIgnored_);
            packetIgnored_ = 0;
            return res;
        }

    private:
        void request()
        {
            if (first_) {
                std::cerr << "first packet: " << ((void*) this) << std::endl;
                first_ = false;
            }

            senf::EthernetPacket const & eth (input());
            std::uint8_t ttl (0);

            // look into the next header and do something
            senf::IPv4Packet const & ipv4 (eth.next<senf::IPv4Packet>(senf::nothrow));
            if (ipv4) {
                ttl = std::uint8_t(ipv4->ttl());
                ttlMin_ = std::min(ttl, ttlMin_);
                ttlMax_ = std::max(ttl, ttlMax_);
            }
            senf::IPv6Packet const & ipv6 (eth.next<senf::IPv6Packet>(senf::nothrow));
            if (ipv6) {
                ttl = std::uint8_t(ipv6->hopLimit());
                ttlMin_ = std::min(ttl, ttlMin_);
                ttlMax_ = std::max(ttl, ttlMax_);
            }

            // only forward EthernetII frames. If a ttl is known, it must be >2
            if ((eth->type_length() > 1536) && ((ttl == 0) || (ttl > 2))) {
                eth->destination() << dst_;
                eth->source() << src_;
                output(eth);
                ++ packetCount_;
            } else {
                ++ packetIgnored_;
            }
        }

        bool first_;
        senf::MACAddress src_;
        senf::MACAddress dst_;
        unsigned packetCount_;
        unsigned packetIgnored_;
        std::uint8_t ttlMin_, ttlMax_;
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
        {
            monitorDevice( monInterfaceName(wnlc_.phyIndex()));

            /*
            maxBurst(32);
            qlen(384);
            sndBuf(32768);
            */
        }

        ~WLANInterface()
        {
            try {
                wnlc_.ibss_leave();
            } catch (...) {}
        }

        std::string initInterfaces(std::string const & dev)
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
            
            return ifname;
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
                ->ampduFactor(1);  // WiBACK default. Should be configurable via policy.
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
        DecoInterface iface0;
        DecoInterface iface1;
        Forwarder forwarder0;
        Forwarder forwarder1;

        ppi::IntervalTimer statsTimer;
        struct ::timeval startTime;
        unsigned long lastStats;

        senf::scheduler::SignalEvent sigint;
        senf::scheduler::SignalEvent sigterm;

        App(std::string const & iface0_, 
            unsigned freq0,
            senf::MACAddress peer0,
            std::string const & iface1_,
            unsigned freq1,
            senf::MACAddress peer1,
            unsigned burst_,
            unsigned ring_,
            unsigned rxbuf_,
            unsigned txbuf_)
            : iface0 (iface0_),
              iface1 (iface1_),
              forwarder0 (iface1.id(), peer1),
              forwarder1 (iface0.id(), peer0),
              statsTimer (senf::ClockService::seconds(1)),
              lastStats (0),
              sigint (SIGINT, boost::bind(&App::terminate, this)),
              sigterm (SIGTERM, boost::bind(&App::terminate, this))
        {            
            iface0.interface().rcvBuf(rxbuf_);
            iface0.interface().sndBuf(txbuf_);
            iface0.interface().qlen(ring_);
            iface0.interface().maxBurst(burst_);
            
            iface1.interface().rcvBuf(rxbuf_);
            iface1.interface().sndBuf(txbuf_);
            iface1.interface().qlen(ring_);
            iface1.interface().maxBurst(burst_);

            iface0.interface().enable();
            iface1.interface().enable();

            WLANInterface *wif;
            if ((wif = dynamic_cast<WLANInterface*>(&iface0.interface())) != NULL) {
                wif->join(freq0, 40, 4711u);
            }
            if ((wif = dynamic_cast<WLANInterface*>(&iface1.interface())) != NULL) {
                wif->join(freq1, 40, 4711u);
            }

            ppi::connect(iface0, forwarder0);
            ppi::connect(forwarder0, iface1);

            ppi::connect(iface1, forwarder1);
            ppi::connect(forwarder1, iface0);

            registerEvent(statsTimer, &App::stats);
            ::gettimeofday(&startTime, 0);
        }

        void stats()
        {
            struct ::timeval tv;
            ::gettimeofday(&tv, 0);
            unsigned long time (
                (tv.tv_sec - startTime.tv_sec) * 1000000 + tv.tv_usec - startTime.tv_usec);
            std::cout << tv.tv_sec << '.' << std::setw(6) << std::setfill('0') << tv.tv_usec
                      << " usecs " << time - lastStats
                      << " packets forwarded " << forwarder0.packetCount()   + forwarder1.packetCount()
                      << " packets ignored "   << forwarder0.packetIgnored() + forwarder1.packetIgnored()
                      << " queue0-drops " << iface0.interface().rxQueueDropped()
                      << " queue1-drops " << iface1.interface().rxQueueDropped()
                      << std::endl;
            lastStats = time;
        }

        void terminate()
        {
            std::cerr << "bye." << std::endl;
            senf::scheduler::terminate();
        }
    };

    template <class Application>
    void run(std::string iface0, unsigned freq0, senf::MACAddress peer0, 
             std::string iface1, unsigned freq1, senf::MACAddress peer1, 
             unsigned burst, unsigned nrring, unsigned rxbuf, unsigned txbuf)
    {
        Application app (iface0, freq0, peer0, iface1, freq1, peer1, burst, nrring, rxbuf, txbuf);
        ppi::run();
    }

}

int main(int argc, char ** argv)
{
//    senf::log::ConsoleTarget::logToStderr();

    if (argc < 9) {
        std::cerr << "Usage:\n    " << argv[0] << "\n"
                  << "        <iface1> <freq0> <peermac1>\n"
                  << "        <iface2> <freq1> <peermac2>\n"
                  << "        <burst> <rxbuf> <txbuf> <nrring> [[NO]RT] [[NO]WLAN]\n"
                  << std::endl;
        return 1;
    }

    std::string iface0 (argv[1]);
    unsigned freq0 (boost::lexical_cast<unsigned>(argv[2]));
    senf::MACAddress peer0 (senf::MACAddress::from_string(argv[3]));
    std::string iface1 (argv[4]);
    unsigned freq1 (boost::lexical_cast<unsigned>(argv[5]));
    senf::MACAddress peer1 (senf::MACAddress::from_string(argv[6]));

    unsigned burst (boost::lexical_cast<unsigned>(argv[7]));
    unsigned rxbuf (boost::lexical_cast<unsigned>(argv[8]));
    unsigned txbuf (boost::lexical_cast<unsigned>(argv[9]));
    unsigned nrring (boost::lexical_cast<unsigned>(argv[10]));

    bool enableRT (true);
    bool useWlan (false);

    for (int i (11); i < argc; ++ i) {
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
        ::sched_param schedpm = {};
        schedpm.sched_priority = 1;
        if (::sched_setscheduler(::getpid(), SCHED_FIFO, &schedpm) >= 0)
            std::cerr << "SCHED_FIFO realtime scheduling enabled" << std::endl;
  }

    senf::scheduler::watchdogTimeout(0); // allow debugging using gdb

    senf::console::Server::start(senf::INet4SocketAddress(senf::INet4Address::None, 23232u));

    if (useWlan) 
        run< App<WLANInterface> >(iface0, freq0 , peer0, 
                                  iface1, freq1, peer1, 
                                  burst, nrring, rxbuf, txbuf);
        run< App<EthernetInterface> >(iface0, freq0, peer0, 
                                      iface1, freq1, peer1, 
                                      burst, nrring, rxbuf, txbuf);
        
    return 0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "main.mpp"


// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// End:
