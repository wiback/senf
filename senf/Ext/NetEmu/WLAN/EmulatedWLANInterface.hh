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
        virtual void v_id(MACAddress const & eui) override;
        virtual MACAddress v_id() const override;
        std::string const & v_device() const override;

        // modulation
        virtual ModulationParameter::id_t v_modulationId() const override;
        virtual void v_modulationId(ModulationParameter::id_t) override;

        // txpower
        virtual int v_txPower() const override;
        virtual void v_txPower(int power) override;

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
