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
    \brief EmulatedDVBInterface public header */

#ifndef HH_SENF_Ext_NetEmu_DVB_EmulatedDVBInterface_
#define HH_SENF_Ext_NetEmu_DVB_EmulatedDVBInterface_ 1

// Custom includes
#include <senf/Ext/NetEmu/EmulatedWirelessInterface.hh>
#include "DVBInterface.hh"

#include "EmulatedDVBInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class EmulatedDVBInterface
        : public EmulatedWirelessInterface,
          public DVBInterface
    {
    public:
        void registerFrequency(unsigned frequenc);
        void registerFrequency(unsigned frequencyLower, unsigned frequencyUpper);

    protected:
        EmulatedDVBInterface();

        void init();

        virtual void v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned frequency) override;

        virtual void v_id(MACAddress const & id) override;
        virtual MACAddress v_id() const override;
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual bool v_enabled() const override;
        virtual unsigned v_frequency() const override;
        virtual unsigned v_bandwidth() const override;
        virtual void v_frequency(unsigned frequency, unsigned bandwidth) override;
        virtual void v_coverageRange(unsigned distance) override;

        std::string const & v_device() const override;

        ModulationParameter::id_t modulationId_;
        MACAddress id_;
        std::string dev_;
    };


    /** \brief Emulated DVBSender Interface

        \ingroup senf_emu_interface_implementations
     */
    class EmulatedDVBSenderInterface
        : private detail::EmulatedDVBSenderInterfaceNet,
          public EmulatedDVBInterface,
          public WirelessTransmitter, public EmulatedWirelessTransmitter
    {
    public:
        typedef TypedInterfaceDecorator<
            EmulatedDVBSenderInterface, WirelessInterface::Decorator> Decorator;

        EmulatedDVBSenderInterface();

        using WirelessTransmitter::registerTxPower;
        using EmulatedDVBInterface::registerFrequency;

    private:
        virtual ModulationParameter::id_t v_modulationId() const override;
        virtual void v_modulationId(ModulationParameter::id_t id) override;
        virtual int v_txPower() const override;
        virtual void v_txPower(int power) override;
        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;
        virtual void v_initEmulationInterface() override;
        virtual void v_deinitEmulationInterface() override;

        int power_;

        friend struct detail::EmulatedDVBSenderInterfaceNet;
    };


    /** \brief Emulated DVBReceiver Interface

        \ingroup senf_emu_interface_implementations
     */
    class EmulatedDVBReceiverInterface
        : private detail::EmulatedDVBReceiverInterfaceNet,
          public EmulatedDVBInterface,
          public WirelessReceiver, public EmulatedWirelessReceiver
    {
    public:
        typedef TypedInterfaceDecorator<
            EmulatedDVBReceiverInterface, WirelessInterface::Decorator> Decorator;

        EmulatedDVBReceiverInterface();

        using EmulatedDVBInterface::registerFrequency;

    private:
        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;
        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool v) override;
        virtual void v_mcAdd(MACAddress const & address) override;
        virtual void v_mcDrop(MACAddress const & address) override;
        virtual void v_initEmulationInterface() override;
        virtual void v_deinitEmulationInterface() override;

        friend struct detail::EmulatedDVBReceiverInterfaceNet;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmulatedDVBInterface.cci"
//#include "EmulatedDVBInterface.ct"
//#include "EmulatedDVBInterface.cti"
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
