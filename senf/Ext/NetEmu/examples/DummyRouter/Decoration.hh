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
    \brief Decoration public header */

#ifndef HH_Carmen_Decoration_
#define HH_Carmen_Decoration_ 1

// Custom includes
#include <senf/PPI.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/InterfaceAPI.hh>
#include <senf/Utils/Statistics.hh>

//#include "Decoration.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace wiback {

    class Config;

    class PacketCounter
        : public senf::ppi::module::MonitorModule<>
    {
        SENF_PPI_MODULE(PacketCounter);
    public:
        PacketCounter();

        unsigned count() const;

    private:
        void v_handlePacket(senf::Packet const & p);

        unsigned count_;
    };

    class InterfaceAnnotater
        : public senf::ppi::module::Module
    {
            SENF_PPI_MODULE(InterfaceAnnotater);
    public:
        senf::ppi::connector::PassiveInput<> input;
        senf::ppi::connector::ActiveOutput<> output;

        explicit InterfaceAnnotater(senf::emu::Interface & interface);

    private:
        void request();

        senf::emu::Interface & interface_;
    };

    struct ReceiveGroup
    {
        PacketCounter counter;
        senf::ppi::module::RateAnalyzer rateAnalyzer;
        InterfaceAnnotater annotator;

        senf::ppi::connector::PassiveInputJack<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutputJack<senf::EthernetPacket> output;

        //-////////////////////////////////////////////////////////////////////////

        ReceiveGroup(senf::emu::Interface & interface);

        void init(Config & config);
        void startStatistics(senf::ClockService::clock_type pollInterval);

        //-////////////////////////////////////////////////////////////////////////

        unsigned receivedPackets() const;
    };

    struct TransmitGroup
    {
        PacketCounter counter;
        senf::ppi::module::RateAnalyzer rateAnalyzer;

        senf::ppi::connector::PassiveInputJack<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutputJack<senf::EthernetPacket> output;

        typedef boost::ptr_vector<senf::Statistics> Statistics;
        Statistics statistics;

        //-////////////////////////////////////////////////////////////////////////

        TransmitGroup(senf::emu::Interface & interface);

        void init(Config & config);
        void startStatistics(senf::ClockService::clock_type pollInterval);

        //-////////////////////////////////////////////////////////////////////////

        unsigned sentPackets() const;

        senf::MACAddress peer;
    };

    typedef senf::emu::ApplyInterfaceDecorator<ReceiveGroup, TransmitGroup> CARMENDecoration;

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Decoration.cci"
//#include "Decoration.ct"
//#include "Decoration.cti"
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
