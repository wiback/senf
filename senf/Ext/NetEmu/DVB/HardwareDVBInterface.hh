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
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual bool v_enabled() const override;

        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;

        virtual void v_id(MACAddress const & id) override;
        virtual MACAddress v_id() const override;

        virtual int v_txPower() const override;
        virtual void v_txPower(int power) override;

        virtual ModulationParameter::id_t v_modulationId() const override;
        virtual void v_modulationId(ModulationParameter::id_t id) override;

        std::string const & v_device() const override;

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
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual bool v_enabled() const override;

        virtual void v_id(MACAddress const & id) override;
        virtual MACAddress v_id() const override;
        
        std::string const & v_device() const override;

        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;

        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool a) override;

        virtual void v_mcAdd(MACAddress const & address) override;
        virtual void v_mcDrop(MACAddress const & address) override;

        virtual void v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned frequency) override;
        void tuneEvent(const struct dvb_frontend_event & ev);
        void tune(std::string const & channelName);
        using DVBInterface::tune;

        int tsPidChanged(int oldPid);

        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory_;
        MACAddress id_;

        int tsPid_;
        int stuffingPid_;
        DVBSocketController dvbController;
        std::string dev_;

        friend struct detail::HardwareDVBReceiverInterfaceNet;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "HardwareDVBInterface.cci"
//#include "HardwareDVBInterface.ct"
//#include "HardwareDVBInterface.cti"
#endif
