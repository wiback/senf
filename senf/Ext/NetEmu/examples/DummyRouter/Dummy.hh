// $Id:$
//
// Copyright (C) 2008
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Dummy public header */

#ifndef HH_Carmen_Dummy_
#define HH_Carmen_Dummy_ 1

// Custom includes
#include <boost/random.hpp>
#include <senf/PPI.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include "Config.hh"
#include "Setup.hh"
#include "Decoration.hh"

//#include "Dummy.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace wiback {

    SENF_LOG_DEFINE_STREAM( PacketLog, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE );

    /** \brief Dummy routing module

        This module will dump all packets received on \a input and will periodically generate
        packets on \a output. The packets will be sent to a random interface.
     */
    class DummyRouter
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE( DummyRouter );
        SENF_LOG_DEFAULT_STREAM( PacketLog );
        SENF_LOG_CLASS_AREA();

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        DummyRouter(Config & config, Setup<CARMENDecoration> & setup);

        unsigned delayedTicks() const;
        senf::ClockService::clock_type maxTickDelay() const;

        void start();

    private:
        void request();
        void tick();
        void sendPacket();

        Config & config_;
        Setup<CARMENDecoration> & setup_;
        senf::ppi::IntervalTimer timer_;

        union Counter {
            boost::uint32_t value;
            boost::uint8_t bytes[4];
        };
        Counter counter_;

        boost::uniform_smallint<> random_;
        boost::mt19937 generator_;
        unsigned delayedTicks_;
        senf::ClockService::clock_type maxTickDelay_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Dummy.cci"
//#include "Dummy.ct"
//#include "Dummy.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
