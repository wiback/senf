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

#include "Configuration.hh"
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
        
        senf::scheduler::TimerEvent timer_;
        senf::ClockService::clock_type startTime;
        senf::ClockService::clock_type duration_;
        senf::ClockService::clock_type period;
        senf::ClockService::clock_type nextTime;
        senf::scheduler::SignalEvent sigint;
        senf::scheduler::SignalEvent sigterm;
        
        App(std::string const & iface_, senf::ClockService::clock_type const & duration, unsigned burst, unsigned qlen, unsigned txbuf)
            : iface (iface_),
              timer_("APP", senf::membind(&App::timerEvent, this), senf::ClockService::clock_type(0), false),
              startTime(senf::scheduler::now()),
              duration_(duration),
              period(senf::ClockService::seconds(1)),
              nextTime(startTime + period),
              sigint (SIGINT, boost::bind(&App::terminate, this)),
              sigterm (SIGTERM, boost::bind(&App::terminate, this))
        {
            iface.interface().maxBurst(burst);
            iface.interface().qlen(qlen);
            iface.interface().sndBuf(txbuf);            
            iface.interface().enable();

            timer_.timeout(nextTime);            
        }
        
        virtual void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) = 0;
        
        void timerEvent()
        {
            if (duration_ and (senf::scheduler::now() - startTime) >= duration_){
                terminate();
            }
            
            v_stats(nextTime - startTime, period);
            nextTime += period;
            timer_.timeout(nextTime);
        };
        
        void terminate()
        {
            senf::scheduler::terminate();
        }
    };
    
    template <class Interface>
    class AppRx : public App<Interface>
    {
    public:
        Receiver receiver;
        
        AppRx(std::string const & iface_, senf::ClockService::clock_type const & duration, unsigned freq, unsigned width, senf::MACAddress peer,
              unsigned burst, unsigned qlen, unsigned txbuf, unsigned bandwidth)
            : App<Interface> (iface_, duration, burst, qlen, txbuf),
            receiver(peer, App<Interface>::iface.interface().id())
        {
            ppi::connect(App<Interface>::iface, receiver);

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(freq, width, 4711u);
            }

            std::cerr << "Receiver ready on " << iface_ << ", mac " << App<Interface>::iface.interface().id() << ", peer " << peer << std::endl;
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << " queue-drops " << App<Interface>::iface.interface().rxQueueDropped() << "; ";
            receiver.report(std::cout, period);
            receiver.clear();
            std::cout << std::endl;
        }
    };
    
    template <class Interface>
    class AppTx : public App<Interface>
    {
    public:
        Sender sender;
        
        AppTx(std::string const & iface_, senf::ClockService::clock_type const & duration, unsigned freq, unsigned width, senf::MACAddress peer,
              unsigned burst, unsigned qlen, unsigned txbuf, unsigned bandwidth)
            : App<Interface> (iface_, duration, burst, qlen, txbuf),
            sender(App<Interface>::iface.interface().id(), peer)
        {
            ppi::connect(sender, App<Interface>::iface);            

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(freq, width, 4711u);
            }

            sender.bitrate(bandwidth);
            
            std::cerr << "Transmitter ready on " << iface_ << ", mac " << App<Interface>::iface.interface().id() << ", peer " << peer << ", bitrate " << bandwidth << "kbps" << std::endl;
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << " seqNo " << sender.seqNo;
            std::cout << std::endl;
        }
    };
    
    template <class Application>
    void run(std::string iface, senf::ClockService::clock_type const & duration, unsigned freq, unsigned width, senf::MACAddress peer, 
             unsigned burst, unsigned qlen, unsigned txbuf, unsigned bandwidth)
    {
        Application app (iface, duration, freq, width, peer, burst, qlen, txbuf, bandwidth);
        ppi::run();
    }    
}

int main(int argc, const char ** argv)
{
    senf::log::ConsoleTarget::logToStderr();

    Configuration config;

    if (!config.parse( argc, argv)) {
        exit(1);
    }

    // enable console for (remote) logging
    senf::console::Server::start(senf::INet4SocketAddress(senf::INet4Address::None, 23232u));
    
    if (config.iface.find("phy") == 0){
        if (config.bandwidth == 0)
            run< AppRx<WLANInterface> >(config.iface, config.duration, config.frequency, config.ht40 ? 40 : 20, config.peer,
                                        config.maxBurst, config.qlen, config.txBuf, 0);
        else
            run< AppTx<WLANInterface> >(config.iface, config.duration, config.frequency, config.ht40 ? 40 : 20, config.peer,
                                        config.maxBurst, config.qlen, config.txBuf, config.bandwidth);
    }
    else {
        if (config.bandwidth == 0)
            run< AppRx<EthernetInterface>>(config.iface, config.duration, 0, 0, config.peer,
                                           config.maxBurst, config.qlen, config.txBuf, 0);
        else
            run< AppTx<EthernetInterface> >(config.iface, config.duration, 0, 0, config.peer,
                                            config.maxBurst, config.qlen, config.txBuf, config.bandwidth);
    }
    
    return 0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "main.mpp"


// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// End:
