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
    
    class Receiver : public module::Module
    {
        SENF_PPI_MODULE(Receiver);
    public:
        connector::PassiveInput<senf::EthernetPacket> input;
        std::uint32_t label;
        wiback::TIMSeqNoStats e2eStats;
        wiback::TIMSeqNoStats llStats;
        std::uint32_t ignored;
        
        Receiver(senf::MACAddress const & src, senf::MACAddress const & dst)
            : src_ (src), dst_(dst)
        {
            noroute(input);
            input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
            input.onRequest(&Receiver::request);

            reset();
        }
        
    private:
        void request()
        {
            senf::EthernetPacket const & eth (input());
            if ((!src_ or eth->source() == src_) and eth->destination() == dst_) { 
                senf::MPLSPacket mpls (eth.next<senf::MPLSPacket>(senf::nothrow));
                if (mpls) {
                    if (label == 0xffffffff or label != mpls->label()) {
                        reset();
                        label = mpls->label();
                    }
                    wiback::TIMPacket tim (mpls.next<wiback::TIMPacket>(senf::nothrow));
                    if (tim) {
                        e2eStats.processSeqNo(tim);
                        llStats.processLLSeqNo(tim);
                        return;
                    }
                }
            }
            ignored++;
         }

        void reset() {
            label = 0xffffffff; 
            e2eStats.reset();
            llStats.reset();
            ignored = 0;
        }

    public:
        void clear() {
            e2eStats.clear();
            llStats.clear();
            ignored = 0;
        }

        void report(std::ostream & os, senf::ClockService::clock_type const & period) {
            os << "Label " << label;
            os << "; E2E stats: "; e2eStats.dump(os, period);
            os << "; LL stats: "; llStats.dump(os, period);
            os << "; ignored " << ignored;
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
