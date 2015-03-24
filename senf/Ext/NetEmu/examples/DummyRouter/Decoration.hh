// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
