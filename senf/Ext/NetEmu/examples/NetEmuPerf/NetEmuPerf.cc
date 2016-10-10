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

#include "Interfaces.hh"
#include "Rx.hh"
#include "Tx.hh"

//#include "main.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;
namespace emu = senf::emu;

namespace {
        
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
        
        App(std::string const & iface_, unsigned burst, unsigned qlen, unsigned txbuf)
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
        
        AppRx(std::string const & iface_, unsigned freq, unsigned width, senf::MACAddress peer, unsigned burst, unsigned qlen, unsigned txbuf)
            : App<Interface> (iface_, burst, qlen, txbuf),
            receiver(peer, App<Interface>::iface.interface().id())
        {
            ppi::connect(App<Interface>::iface, receiver);

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(freq, width, 4711u);
            }
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
        
        AppTx(std::string const & iface_, unsigned freq, unsigned width, senf::MACAddress peer, unsigned burst, unsigned qlen, unsigned txbuf)
            : App<Interface> (iface_, burst, qlen, txbuf),
            sender(App<Interface>::iface.interface().id(), peer)
        {
            ppi::connect(sender, App<Interface>::iface);            

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(freq, width, 4711u);
            }
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << std::endl;
        }
    };
    
    template <class Application>
    void run(std::string iface, unsigned freq, unsigned width, senf::MACAddress peer, 
             unsigned burst, unsigned qlen, unsigned txbuf)
    {
        Application app (iface, freq, width, peer, burst, qlen, txbuf);
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
        run<AppRx<WLANInterface> >(iface, freq, 20u, peer,
                                   burst, qlen, txbuf);
    else
        run<AppRx<EthernetInterface> >(iface, freq, 20u, peer, 
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
