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

        std::string const & v_device() const override;

        void init();
        //TODO: limit vlanID to 12 usable bits
        void vlan(boost::uint16_t id);
        boost::uint16_t vlan() const;

    private:
        // interface
        virtual void v_id(MACAddress const & eui) override;
        virtual MACAddress v_id() const override;

        INet4Network vlanGroupRange_;
        unsigned portbase_;
        MACAddress id_;
        boost::uint16_t vlanId_;
        std::string dev_;

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
