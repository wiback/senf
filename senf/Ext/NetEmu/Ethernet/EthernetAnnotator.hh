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
//       Christian Niephaus <cni@berlios.de>

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
