// $Id:$
//
// Copyright (C) 2013
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
    \brief HardwareDVBInterface public header */

#ifndef HH_SENF_Ext_NetEmu_DVB_HardwareDVBInterface_
#define HH_SENF_Ext_NetEmu_DVB_HardwareDVBInterface_ 1

// Custom includes
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Ext/NetEmu/InterfaceId.hh>
#include "DVBInterface.hh"

#include "HardwareDVBInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class HardwareDVBInterface
        : public DVBInterface, public HardwareInterface
    {
    protected:
        HardwareDVBInterface();

        void init();

        virtual void v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned frequency);

        ModulationParameter::id_t modulationId_;

    private:
        void registerFrequencyRange(unsigned frequencyLower, unsigned frequencyUpper);
        virtual unsigned v_frequency() const;
        virtual unsigned v_bandwidth() const;
        virtual void v_frequency(unsigned freq, unsigned bw);

        virtual void v_coverageRange(unsigned distance);

        unsigned frequency_;
        unsigned bandwidth_;
    };


    class HardwareDVBSenderInterface
        : private detail::HardwareDVBSenderInterfaceNet,
          public HardwareDVBInterface,
          public WirelessTransmitter
    {
    public:
        HardwareDVBSenderInterface(std::string const & iface);

    private:
        virtual void v_enable();
        virtual void v_disable();
        virtual bool v_enabled() const;

        virtual unsigned v_mtu() const;
        virtual void v_mtu(unsigned v);

        virtual void v_id(MACAddress const & id);
        virtual MACAddress v_id() const;

        virtual int v_txPower() const;
        virtual void v_txPower(int power);

        virtual ModulationParameter::id_t v_modulationId() const;
        virtual void v_modulationId(ModulationParameter::id_t id);

        std::string iface_;
        NetdeviceController ctrl_;
        int txPower_;
    };


    class HardwareDVBReceiverInterface
        : private detail::HardwareDVBReceiverInterfaceNet,
          public HardwareDVBInterface,
          public WirelessReceiver
    {
    public:
        HardwareDVBReceiverInterface();

        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory();
        void ifaceIdFactory(boost::shared_ptr<InterfaceIdFactoryBase> factory);

    private:
        virtual void v_enable();
        virtual void v_disable();
        virtual bool v_enabled() const;

        virtual void v_id(MACAddress const & id);
        virtual MACAddress v_id() const;

        virtual bool v_promisc() const;
        virtual void v_promisc(bool v);

        virtual bool v_annotationMode() const;
        virtual void v_annotationMode(bool a);

        virtual void v_mcAdd(MACAddress const & address);
        virtual void v_mcDrop(MACAddress const & address);

        virtual void v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned frequency);
        void tuneEvent(const struct dvb_frontend_event & ev);
        void tune(std::string const & channelName);
        using DVBInterface::tune;

        int tsPidChanged(int oldPid);

        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory_;
        MACAddress id_;

        int tsPid_;
        int stuffingPid_;
        DVBSocketController dvbController;

        friend struct detail::HardwareDVBReceiverInterfaceNet;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "HardwareDVBInterface.cci"
//#include "HardwareDVBInterface.ct"
//#include "HardwareDVBInterface.cti"
#endif
