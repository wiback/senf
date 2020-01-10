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
