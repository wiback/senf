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
    \brief EmulatedWLANInterface public header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_EmulatedWLANInterface_
#define HH_SENF_Ext_NetEmu_WLAN_EmulatedWLANInterface_ 1

// Custom includes
#include <senf/Ext/NetEmu/InterfaceAPIBase.hh>
#include "WLANInterface.hh"

#include "EmulatedWLANInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Emulated WLAN Interface

        An EmulatedWLANInterface represents implements the WirelessInterface, WirelessTransmitter
        and WirelessReceiver API's in an EmulatedInterface.

        \ingroup senf_emu_interface_implementations
     */
    class EmulatedWLANInterface
        : private detail::EmulatedWLANInterfaceNet,
          public InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Emulated, WLANInterface>
    {
        typedef InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Emulated, WLANInterface> Base;

    public:
        typedef TypedInterfaceDecorator<EmulatedWLANInterface, WirelessInterface::Decorator> Decorator;
        enum Mode { ADHOC, STA, AP };

        using WirelessInterface::registerFrequency;
        using WirelessTransmitter::registerTxPower;

        EmulatedWLANInterface();
        void init();
        void mode(Mode);
        Mode mode();
        using detail::EmulatedWLANInterfaceNet::beaconInterval;

        void registerModulation(ModulationParameter::id_t id);

    private:
        void registerVHTModulation(unsigned vhtMcsIndex, unsigned streams, unsigned bandwidth, bool shortGI);
        void registerHTModulation(unsigned mcsIndex);
        void registerLegacyModulation(unsigned rate);
        void registerModulation(WLANModulationParameter::Type type, std::vector<std::string> args);

        // interface
        virtual void v_id(MACAddress const & eui);
        virtual MACAddress v_id() const;
        std::string const & v_device() const override;

        // modulation
        virtual ModulationParameter::id_t v_modulationId() const;
        virtual void v_modulationId(ModulationParameter::id_t);

        // txpower
        virtual int v_txPower() const;
        virtual void v_txPower(int power);

        MACAddress id_;
        ModulationParameter::id_t modulationId_;
        int power_;
        Mode mode_;
        std::string dev_;

        friend struct detail::EmulatedWLANInterfaceNet;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmulatedWLANInterface.cci"
//#include "EmulatedWLANInterface.ct"
//#include "EmulatedWLANInterface.cti"
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
