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
    \brief HardwareEthernetInterface internal header */

#ifndef IH_SENF_Ext_NetEmu_Ethernet_EthernetAnnotator_
#define IH_SENF_Ext_NetEmu_Ethernet_EthernetAnnotator_ 1

// Custom includes
#include <senf/PPI/QueueSocketSourceSink.hh>
#include <senf/Ext/NetEmu/VLanId.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    typedef std::function<void (senf::EthernetPacket const & eth)> HandleEthPkt;
    
    class EthernetAnnotator
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(EthernetAnnotator);

    public:
        ppi::connector::PassiveInput<senf::EthernetPacket> input;
        ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetAnnotator(bool rxMode, bool mmapMode, senf::MACAddress const & id_ = senf::MACAddress::None);

        void id(MACAddress const & id);
        MACAddress const & id() const;

        void promisc(bool p);
        void insertTag(VLanId const & pvid);
        void removeTag(VLanId const & pvid);
        void clearTag();
        void annotate(bool a);
        bool annotate() const;

        std::uint32_t vlanMismatch();

    private:
        void requestRx();
        void requestRxMMAP();
        void requestRxMMAPpromisc();
        void requestTx();

        void netemu_annotations(senf::EthernetPacket const & eth);

        void handle_pkt_dummy(senf::EthernetPacket const & eth);
        void handle_pkt_dummy_annotate(senf::EthernetPacket const & eth);
        void handle_pkt_insert_tag(senf::EthernetPacket const & eth);
        void handle_pkt_remove_tag(senf::EthernetPacket const & eth);

        MACAddress id_;
        VLanId pvid_;
        std::uint32_t vlanMismatch_;
        HandleEthPkt handle_pkt;
        bool annotate_;
        bool rxMode_;
        bool mmapMode_;
    };
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
