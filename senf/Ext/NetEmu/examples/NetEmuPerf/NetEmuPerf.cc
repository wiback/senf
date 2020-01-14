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
        
        App(Configuration const & config)
            : iface (config.iface),
              timer_("APP", senf::membind(&App::timerEvent, this), senf::ClockService::clock_type(0), false),
              startTime(senf::scheduler::now()),
              duration_(config.duration),
              period(config.reportingInterval),
              nextTime(startTime + period),
              sigint (SIGINT, boost::bind(&App::terminate, this)),
              sigterm (SIGTERM, boost::bind(&App::terminate, this))
        {
            iface.interface().maxBurst(config.maxBurst);
            iface.interface().qlen(config.qlen);
            iface.interface().sndBuf(config.txBuf);            
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
        
        AppRx(Configuration const & config)
            : App<Interface> (config),
            receiver(config.peer, App<Interface>::iface.interface().id())
        {
            ppi::connect(App<Interface>::iface, receiver);

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(config.frequency, config.ht40 ? 40 : 20, 4711u);
            }

            std::cerr << "Receiver ready on " << config.iface << ", mac " << App<Interface>::iface.interface().id() << ", peer " << config.peer << std::endl;
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
        
        AppTx(Configuration const & config)
            : App<Interface> (config),
            sender(App<Interface>::iface.interface().id(), config.peer, config.label)
        {
            ppi::connect(sender, App<Interface>::iface);            

            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                wif->join(config.frequency, config.ht40 ? 40 : 20, 4711u);
            }

            sender.bitrate(config.bitrate);
            
            std::cerr << "Transmitter ready on " << config.iface << ", mac " << App<Interface>::iface.interface().id()
                      << ", peer " << config.peer << ", label " << config.label << ", bitrate " << config.bitrate << "kbps" << std::endl;
        }
        
        void v_stats(senf::ClockService::clock_type const & tstamp, senf::ClockService::clock_type const & period) {
            std::cout << senf::ClockService::in_seconds(tstamp) << '.' << std::setw(3) << std::setfill('0') << (senf::ClockService::in_milliseconds(tstamp) % 1000);
            std::cout << " seqNo " << sender.seqNo;
            WLANInterface *wif (dynamic_cast<WLANInterface*>(&App<Interface>::iface.interface()));
            if (wif) {
                std::cout << ", mmStats ";
                wif->dumpMmapStats(std::cout);
            }
            EthernetInterface *eif (dynamic_cast<EthernetInterface*>(&App<Interface>::iface.interface()));
            if (eif) {
                std::cout << ", mmStats ";
                eif->dumpMmapStats(std::cout);
            }
            std::cout << std::endl;
        }
    };
    
    template <class Application>
    void run(Configuration const & config)
    {
        Application app (config);
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
    if (config.consolePort > 0) {
        senf::console::Server::start(senf::INet4SocketAddress(senf::INet4Address::None, config.consolePort));
    }
    
    if (config.iface.find("phy") == 0){
        if (config.bitrate == 0)
            run< AppRx<WLANInterface> >(config);
        else
            run< AppTx<WLANInterface> >(config);
    }
    else {
        if (config.bitrate == 0)
            run< AppRx<EthernetInterface>>(config);
        else
            run< AppTx<EthernetInterface> >(config);
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
