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

/** \file
    \brief EthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_EthernetInterface_
#define HH_SENF_Ext_NetEmu_Ethernet_EthernetInterface_ 1

// Custom includes
#include "senf/Ext/NetEmu/InterfaceAPIBase.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class EthernetInterface
        : public BidirectionalWiredInterface
    {
    public:
        template <class Output, class Input>
        EthernetInterface(Output & output, Input & input);

    protected:
        void init();

    private:
        virtual std::uint8_t v_linkTypeId() const;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EthernetInterface.cci"
//#include "EthernetInterface.ct"
#include "EthernetInterface.cti"
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
