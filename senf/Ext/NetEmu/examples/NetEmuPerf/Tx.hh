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

#include "MPLSPacket.hh"
#include "TIMPacket.hh"

//#include "main.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;
namespace ppi = senf::ppi;
namespace emu = senf::emu;

namespace {
    
    class Sender : public module::Module
    {
        SENF_PPI_MODULE(Sender);
    public:
        connector::ActiveOutput<senf::EthernetPacket> output; 
        senf::scheduler::TimerEvent timer_;
        senf::EthernetPacket eth;
       
        Sender(senf::MACAddress const & src, senf::MACAddress const & dst)
            : timer_("TX", senf::membind( &Sender::timerEvent, this), senf::ClockService::clock_type(0), false),
              src_(src), dst_ (dst)
        {
            noroute(output);

            eth = senf::EthernetPacket::create();
            eth->source() = src_;
            eth->destination() = dst_;
            senf::MPLSPacket mpls (senf::MPLSPacket::createAfter(eth));
            mpls->label() = 4711;
            mpls->tc() = 1;
            wiback::TIMPacket tim (wiback::TIMPacket::createAfter(mpls));
            senf::DataPacket data (senf::DataPacket::createAfter(tim, 1500u - mpls.size()));
            eth.finalizeAll();
        }
        
    private:
        void timerEvent()
        {
            output(eth.clone());
        }

        void bitrate(unsigned brate_in_kbps)
        {
        }
        
        senf::MACAddress src_;
        senf::MACAddress dst_;
    };
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "main.mpp"


// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// End:
