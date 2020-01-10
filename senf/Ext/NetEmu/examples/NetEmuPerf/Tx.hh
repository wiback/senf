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
#include <senf/Ext/NetEmu/Ethernet/HardwareEthernetInterface.hh>
#include <senf/Ext/NetEmu/WLAN/HardwareWLANInterface.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
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
        senf::MPLSPacket mpls;
        wiback::TIMPacket tim;
        senf::ClockService::clock_type nextTimeout;
        senf::ClockService::clock_type period;
        unsigned seqNo;
        
        Sender(senf::MACAddress const & src, senf::MACAddress const & dst, std::uint32_t label)
            : timer_("TX", senf::membind(&Sender::timerEvent, this), senf::ClockService::clock_type(0), false),
              eth(senf::EthernetPacket::create()),
              mpls(senf::MPLSPacket::createAfter(eth)),
              tim(wiback::TIMPacket::createAfter(mpls)),
              src_(src), dst_(dst)
        {
            noroute(output);

            eth->source() = src_;
            eth->destination() = dst_;
            mpls->label() = label;
            mpls->tc() = 1;
            // let's use non-jumbo frames for now
            senf::DataPacket data (senf::DataPacket::createAfter(tim, 1500u - mpls.size()));
            eth.finalizeAll();
        }
        
    private:
        void timerEvent()
        {
            tim->timestamp() = senf::ClockService::in_milliseconds(senf::scheduler::now()) % (wiback::TIMPacketParser::timestamp_t::max_value + 1);
            tim->sequenceNumber() = seqNo;
            tim->linkLocalSeqNo() = seqNo;
            seqNo = (seqNo + 1) % (wiback::TIMPacketParser::sequenceNumber_t::max_value + 1);

            output(eth.clone());
            nextTimeout += period;
            timer_.timeout(nextTimeout);
        }

    public:
        void bitrate(unsigned brate_in_kbps)
        {
            if (brate_in_kbps == 0) {
                timer_.disable();
                return;
            }

            seqNo = 0;
            nextTimeout = senf::scheduler::now();
            period = senf::ClockService::microseconds((eth.size()*8) * 1000 / brate_in_kbps);
            timerEvent();
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
