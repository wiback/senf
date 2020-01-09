// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//       Thorsten Horstmann <tho@berlios.de>

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
