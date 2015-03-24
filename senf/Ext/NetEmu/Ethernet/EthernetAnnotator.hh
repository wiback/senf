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
//       Christian Niephaus <cni@berlios.de>

/** \file
    \brief HardwareEthernetInterface internal header */

#ifndef IH_SENF_Ext_NetEmu_EthernetAnnotator_
#define IH_SENF_Ext_NetEmu_EthernetAnnotator 1

// Custom includes
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/PPI/QueueSocketSourceSink.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class EthernetAnnotator
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(EthernetAnnotator);

    public:
        ppi::connector::PassiveInput<senf::EthernetPacket> input;
        ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetAnnotator(senf::MACAddress const & id_ = senf::MACAddress::None);

        void id(MACAddress const & id);
        MACAddress const & id() const;

    private:
        void request();

        MACAddress id_;
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
