// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

#ifndef HH_WiBACK_Core_Interfaces_EthernetFragmentation_
#define HH_WiBACK_Core_Interfaces_EthernetFragmentation_ 1

// Custom includes
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include "EthernetFragmentPacket.hh"

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    class EthernetFragmenter
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE( EthernetFragmenter );

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetFragmenter(unsigned fragmentationThreshold = 1500u);

        void fragmentationThreshold(unsigned _fragmentationThreshold);
        unsigned fragmentationThreshold() const;

        unsigned fragmentationCount();

        static bool fragmentationRequired(senf::EthernetPacket const & pkt, unsigned fragmentationThreshold);
        void fragmentFrame(senf::EthernetPacket const & pkt, unsigned fragmentationThreshold, std::vector<senf::EthernetPacket> & frags);
        void fragmentFrame(senf::EthernetPacket const & pkt);

    private:
        void onRequest();

        unsigned fragmentationThreshold_;
        unsigned fragmentationCount_;
    };


    class EthernetReassembler
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE( EthernetReassembler );

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetReassembler();

        unsigned fragmentationCount();

    private:
        static bool isFragmentedPacket(senf::EthernetPacket const & eth);
        void onRequest();

        std::uint8_t nextFragmentNr_;
        unsigned fragmentationCount_;
        senf::EthernetPacket fragmentedPacket_;
        senf::PacketData::iterator payloadIter_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetFragmenter.cci"
//#include "EthernetFragmenter.ct"
//#include "EthernetFragmenter.cti"
#endif
