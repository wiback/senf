// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

#ifndef HH_WiBACK_Core_Interfaces_EthernetReassembler_
#define HH_WiBACK_Core_Interfaces_EthernetReassembler_ 1

// Custom includes
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include "EthernetFragmentPacket.hh"

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    class EthernetReassemblerBase
    {
    public:
        EthernetReassemblerBase();
        virtual ~EthernetReassemblerBase() {};

        bool processFrame(senf::EthernetPacket const & eth, EthernetFragmentPacket const & fragment);
        senf::EthernetPacket & reassembledPacket();

        unsigned fragmentsInvalid();
        unsigned fragmentsProcessed();
        unsigned packetsUnfragmented();
        unsigned packetsReassembled();

    private:
        std::uint8_t nextFragmentNr_;
        senf::EthernetPacket reassembledPacket_;
        senf::PacketData::iterator payloadIter_;

        unsigned packetsUnfragmented_;
        unsigned packetsReassembled_;
        unsigned fragmentsInvalid_;
        unsigned fragmentsProcessed_;
    };

    class EthernetReassembler : 
        public EthernetReassemblerBase
    {
    public:
        EthernetReassembler();
    };

    class EthernetReassemblerModule
        : public EthernetReassemblerBase,
          public senf::ppi::module::Module
    {
        SENF_PPI_MODULE( EthernetReassemblerModule );

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetReassemblerModule();
        
    private:
        void onRequest();
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetFragmenter.cci"
//#include "EthernetFragmenter.ct"
//#include "EthernetFragmenter.cti"
#endif
