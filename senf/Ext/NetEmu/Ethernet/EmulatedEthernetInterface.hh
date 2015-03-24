// $Id:$
//
// Copyright (C) 2010
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
//       Christian Niephaus <christian.niephaus@fokus.fraunhofer.de>

/** \file
    \brief EmulatedEthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_EmulatedEthernetInterface_
#define HH_SENF_Ext_NetEmu_Ethernet_EmulatedEthernetInterface_ 1

// Custom includes
#include "EthernetInterface.hh"

#include "EmulatedEthernetInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Emulated Ethernet Interface

        An EmulatedEthernetInterface represents implements the WiredInterface, WiredTransmitter
        and WiredReceiver API's in an EmulatedInterface.

        \ingroup senf_emu_interface_implementations
     */
    class EmulatedEthernetInterface
        : private detail::EmulatedEthernetInterfaceNet,
          public InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Emulated, EthernetInterface>
  {
      typedef InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Emulated, EthernetInterface> Base;

    public:
        typedef TypedInterfaceDecorator<
            EmulatedEthernetInterface, WiredInterface::Decorator> Decorator;

        EmulatedEthernetInterface();

        void init();
        //TODO: limit vlanID to 12 usable bits
        void vlan(boost::uint16_t id);
        boost::uint16_t vlan() const;

    private:
        // interface
        virtual void v_id(MACAddress const & eui);
        virtual MACAddress v_id() const;

        INet4Network vlanGroupRange_;
        unsigned portbase_;
        MACAddress id_;
        boost::uint16_t vlanId_;

        friend struct detail::EmulatedEthernetInterfaceNet;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmulatedEthernetInterface.cci"
//#include "EmulatedEthernetInterface.ct"
//#include "EmulatedEthernetInterface.cti"
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
