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
