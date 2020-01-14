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
    \brief PacketDelayer public header */

#ifndef HH_SENF_Ext_NetEmu_Modules_PacketDelayer_
#define HH_SENF_Ext_NetEmu_Modules_PacketDelayer_ 1

// Custom includes
#include <queue>
#include <random>
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include <senf/Scheduler/TimerEvent.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class PacketDelayer
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(PacketDelayer);

    public:
        ppi::connector::PassiveInput<> input;
        ppi::connector::ActiveOutput<> output;

        PacketDelayer();
        PacketDelayer(ClockService::clock_type packetDelay);

        ClockService::clock_type delay() const;
        void delay(ClockService::clock_type delay);

        ClockService::clock_type variation() const;
        void variation(ClockService::clock_type variation);

        template <class ThrottlingDisc>
        void throttlingDisc(ThrottlingDisc const & disc);

    private:
        scheduler::TimerEvent timer_;
        ClockService::clock_type delay_;
        ClockService::clock_type variation_;
        std::unique_ptr<std::mt19937> rndEngine_;
        std::unique_ptr<std::uniform_int_distribution<ClockService::int64_type> > rndDist_;
        std::queue<ClockService::clock_type> queue_;

        void init();
        void enableDelay(bool flag);
        void onRequest_noDelay();
        void onRequest_delay();
        void timeout();
        void throttleNotification();
        void unthrottleNotification();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "PacketDelayer.cci"
//#include "PacketDelayer.ct"
#include "PacketDelayer.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
