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
    \brief HardwareDVBInterface non-inline non-template implementation */

#include "HardwareDVBInterface.hh"

// Custom includes
#include <senf/Utils/Console/Variables.hh>
#include <senf/Ext/NetEmu/Annotations.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::HardwareDVBInterface

prefix_ senf::emu::HardwareDVBInterface::HardwareDVBInterface()
    : frequency_(0), bandwidth_(8000)
{
    // register all known modulations
    for (ModulationParameter::id_t id : DVBModulationParameterRegistry::instance().parameterIds()) {
        registerModulation( id);
    }
    registerFrequencyRange(500000, 750000);
}

prefix_ void senf::emu::HardwareDVBInterface::init()
{
    DVBInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;
    consoleDir().add("registerFrequencyRange", fty::Command(&HardwareDVBInterface::registerFrequencyRange, this) );
}

prefix_ void senf::emu::HardwareDVBInterface::registerFrequencyRange(unsigned frequencyLower, unsigned frequencyUpper)
{
    clearRegisteredFrequencies();
    registerFrequency(frequencyLower, frequencyUpper, bandwidth_, bandwidth_);
}

prefix_ void senf::emu::HardwareDVBInterface::v_coverageRange(unsigned distance)
{
    // not supported by DVB
}

prefix_ unsigned senf::emu::HardwareDVBInterface::v_frequency()
    const
{
    return frequency_;
}

prefix_ unsigned senf::emu::HardwareDVBInterface::v_bandwidth()
    const
{
    return bandwidth_;
}

prefix_ void senf::emu::HardwareDVBInterface::v_frequency(unsigned freq, unsigned bw)
{
    frequency_ = freq;
    bandwidth_ = bw;
}

prefix_ void senf::emu::HardwareDVBInterface::v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
        DVBModulationParameter::guard_interval_t gi, unsigned freq)
{
    ModulationParameter::id_t modId (DVBModulationParameterRegistry::instance().parameterId(fec, mod, gi));
    DVBModulationParameterRegistry::instance().findModulationById(modId); // will throw if id is unknown
    modulationId_ = modId;
    frequency(freq, 8000); // we assume a fixed bandwidth of 8 MHz
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::HardwareDVBSenderInterfaceNet

prefix_ senf::emu::detail::HardwareDVBSenderInterfaceNet::HardwareDVBSenderInterfaceNet(std::string const & iface)
    : sink( socket, ppi::TargetDgramWriter<PacketSocketHandle>(LLSocketAddress(iface)))
{
    ppi::connect(forwardSwitch, sink);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::HardwareDVBSenderInterface

prefix_ senf::emu::HardwareDVBSenderInterface::HardwareDVBSenderInterface(std::string const & iface)
    : HardwareDVBSenderInterfaceNet(iface),
      WirelessTransmitter( HardwareDVBSenderInterfaceNet::forwardSwitch.input),
      iface_(iface), ctrl_(iface), txPower_(0)
{
    HardwareDVBInterface::init();
    WirelessTransmitter::init();
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_enable()
{
    HardwareDVBSenderInterfaceNet::forwardSwitch.forward(true);
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_disable()
{
    HardwareDVBSenderInterfaceNet::forwardSwitch.forward(false);
}

prefix_ bool senf::emu::HardwareDVBSenderInterface::v_enabled()
    const
{
    return HardwareDVBSenderInterfaceNet::forwardSwitch.forward();
}

prefix_ unsigned senf::emu::HardwareDVBSenderInterface::v_mtu()
    const
{
    return ctrl_.mtu();
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_mtu(unsigned v)
{
    ctrl_.mtu(v);
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_id(MACAddress const & mac)
{
    throw senf::SystemException(ENOTSUP);
}

prefix_ senf::MACAddress senf::emu::HardwareDVBSenderInterface::v_id()
    const
{
    return ctrl_.hardwareAddress();
}

prefix_ int senf::emu::HardwareDVBSenderInterface::v_txPower()
    const
{
    return txPower_;
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_txPower(int power)
{
    txPower_ = power;
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::HardwareDVBSenderInterface::v_modulationId()
    const
{
    return modulationId_;
}

prefix_ void senf::emu::HardwareDVBSenderInterface::v_modulationId(ModulationParameter::id_t id)
{
    DVBModulationParameterRegistry::instance().findModulationById( id);  // will throw if id is unknown
    modulationId_ = id;
}

prefix_ std::string const & senf::emu::HardwareDVBSenderInterface::v_device()
    const
{
    return iface_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::ActiveDVBSocketSource

prefix_ senf::emu::detail::ActiveDVBSocketSource::ActiveDVBSocketSource(Handle handle)
    : handle_(handle), event_(handle_, senf::ppi::IOEvent::Read|senf::ppi::IOEvent::Err)
{
    registerEvent( event_, &ActiveDVBSocketSource::read );
    route(event_, output);
}

prefix_ void senf::emu::detail::ActiveDVBSocketSource::read(senf::ppi::IOEvent::Event event)
{
    if (event.events & ppi::IOEvent::Err) {
        SENF_LOG((senf::log::IMPORTANT) ("Error on read event. Ignored."));
        return;
    }

    MPESection mpe;
    try {
        mpe = reader_(handle_);
    } catch (SystemException const & e) {
        switch (e.errorNumber()) {
//        case ECRC:
//            SENF_LOG((senf::log::MESSAGE) ("Last section had a CRC error. Ignored."));
//            return;
        case EOVERFLOW:
            SENF_LOG((senf::log::IMPORTANT) ("Overflow. The filtered data was not read from the buffer in due time. Ignored."));
            return;
        default:
            throw e;
        }
    }

    outputMPE2Eth( mpe);
}

prefix_ void senf::emu::detail::ActiveDVBSocketSource::outputMPE2Eth(MPESection mpein)
{
    if (mpein.size() < MPESectionType::initSize()) {
        SENF_LOG((senf::log::IMPORTANT)("insufficient MPE section packet size: " << mpein.size() << "."));
        return;
    }
    if (mpein.size() != mpein->section_length() + 3) {
        SENF_LOG((senf::log::IMPORTANT)("length field value differ packet size: length=" << mpein->section_length() << " size=" << mpein.size() << "."));
    }
    if (mpein->crc() == mpein.parser().calcCrc()) {
        if (mpein->llc_snap_flag()) {
            try {
                output.write(mpein.find<EthernetPacket>());
            } catch (SystemException const & e) {
                SENF_LOG((senf::log::NOTICE)("discarding invalid MPE section. Size of section is " << mpein.size() << "."));
                SENF_LOG((senf::log::VERBOSE)("The exception thrown was: " << e.what()));
            }
        }
    } else {
        SENF_LOG((senf::log::NOTICE)("discarding MPE section because of CRC mismatch. CRC is" << mpein->crc()));
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::InterfaceAnnotator

prefix_ senf::emu::detail::DVBReceiverInterfaceAnnotator::DVBReceiverInterfaceAnnotator(
        HardwareDVBReceiverInterface const & interface, DVBSocketController const & dvbSocketCtrl)
    : interface_(interface), dvbSocketCtrl_(dvbSocketCtrl), lastSNRUpdate_(0)
{
    route(input, output);
    input.onRequest(&DVBReceiverInterfaceAnnotator::request);
}

#define DEFAULT_DVB_NOISE -110

prefix_ void senf::emu::detail::DVBReceiverInterfaceAnnotator::request()
{
    Packet p (input());
    p.annotation<annotations::Timestamp>().fromWallClock();
    p.annotation<annotations::Interface>().value = interface_.id();
    if (lastSNRUpdate_ + ClockService::seconds(1) < scheduler::now())
        snr_ = dvbSocketCtrl_.signalToNoiseRatio();
    {
        emu::annotations::Quality & q (p.annotation<emu::annotations::Quality>());
        q.noise = DEFAULT_DVB_NOISE;
        q.snr   = snr_;
        q.rssi  = snr_ + q.noise;
        q.flags.frameLength = p.size();
    }
    output(p);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::HardwareDVBReceiverInterfaceNet

prefix_ senf::emu::detail::HardwareDVBReceiverInterfaceNet::HardwareDVBReceiverInterfaceNet()
    : dvbSourceHandle( 0, 0),
      dvbControlHandle( 0, 0),
      dvbStuffingHandle( 0, 0),
      dvbSource( dvbSourceHandle),
      annotator(self(), self().dvbController)
{
    ppi::connect(dvbSource, annotator);
    ppi::connect(annotator, forwardSwitch);
}

prefix_ void senf::emu::detail::HardwareDVBReceiverInterfaceNet::initHandle()
{
    dvbSourceHandle.protocol().setBufferSize( 256*4096);
    dvbSourceHandle.protocol().setSectionFilter( self().tsPid_);
    dvbControlHandle.protocol().setBufferSize( 256*4096);
    dvbControlHandle.protocol().setPESFilter( self().stuffingPid_, DMX_IN_FRONTEND, DMX_OUT_TS_TAP, DMX_PES_OTHER, 0);
//    controlHandle_.protocol().setPESFilter(DEFAULT_DVB_STUFFING_PID,DMX_IN_FRONTEND,DMX_OUT_TS_TAP,DMX_PES_OTHER,DMX_IMMEDIATE_START);
}

prefix_ senf::emu::HardwareDVBReceiverInterface const & senf::emu::detail::HardwareDVBReceiverInterfaceNet::self()
    const
{
    return *static_cast<HardwareDVBReceiverInterface const *>(this);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::HardwareDVBReceiverInterface

prefix_ senf::emu::HardwareDVBReceiverInterface::HardwareDVBReceiverInterface()
    :  WirelessReceiver( HardwareDVBReceiverInterfaceNet::forwardSwitch.output),
       ifaceIdFactory_( new SimpleInterfaceIdFactory()), id_(ifaceIdFactory_->getId()), stuffingPid_(8190),
       dvbController( DVBFrontendHandle(0, 0), membind(&HardwareDVBReceiverInterface::tuneEvent, this)),
       dev_("DVB-Receiver")
{
    HardwareDVBInterface::init();
    WirelessReceiver::init();

    namespace fty = console::factory;
    consoleDir().add("tsPID", fty::Variable(tsPid_)
        .onChange(membind(&HardwareDVBReceiverInterface::tsPidChanged, this)) );
    consoleDir().add("stuffingPID", fty::Variable(stuffingPid_));
    consoleDir().add("dvb", dvbController.dir).doc( "Configure DVB device");
    consoleDir().add("tune", fty::Command(
            SENF_MEMBINDFNP(void, HardwareDVBReceiverInterface, tune, (std::string const &))) );

    dvbController.addToConsole(dvbSourceHandle);
    dvbController.addToConsole(dvbControlHandle);
}

prefix_ int senf::emu::HardwareDVBReceiverInterface::tsPidChanged(int oldPid)
{
    HardwareDVBReceiverInterfaceNet::initHandle();
    return tsPid_;
}

prefix_ bool senf::emu::HardwareDVBReceiverInterface::v_promisc()
    const
{
    return true;
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_promisc(bool v)
{
    if (!v)
        throw InvalidArgumentException("promisc mode cannot be disabled.");
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_annotationMode(bool a)
{
    throw InvalidArgumentException("annotation mode not implemented.");
}

prefix_ bool senf::emu::HardwareDVBReceiverInterface::v_annotationMode()
    const
{
    return false;
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_mcAdd(MACAddress const & address)
{
    throw SystemException(ENOTSUP);
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_mcDrop(MACAddress const & address)
{
    throw SystemException(ENOTSUP);
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_enable()
{
    HardwareDVBReceiverInterfaceNet::forwardSwitch.forward(true);
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_disable()
{
    HardwareDVBReceiverInterfaceNet::forwardSwitch.forward(false);
}

prefix_ bool senf::emu::HardwareDVBReceiverInterface::v_enabled()
    const
{
    return HardwareDVBReceiverInterfaceNet::forwardSwitch.forward();
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_id(MACAddress const & mac)
{
    ifaceIdFactory_->releaseId( id_);
    id_ = mac;
}

prefix_ senf::MACAddress senf::emu::HardwareDVBReceiverInterface::v_id()
    const
{
    return id_;
}

prefix_ std::string const & senf::emu::HardwareDVBReceiverInterface::v_device()
    const
{
    return dev_;
}


prefix_ boost::shared_ptr<senf::emu::InterfaceIdFactoryBase> senf::emu::HardwareDVBReceiverInterface::ifaceIdFactory()
{
    return ifaceIdFactory_;
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::ifaceIdFactory(boost::shared_ptr<InterfaceIdFactoryBase> factory)
{
    ifaceIdFactory_->releaseId( id_);
    ifaceIdFactory_ = factory;
    id_ = MACAddress::None;
    id( ifaceIdFactory_->getId());
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::tuneEvent(struct dvb_frontend_event const & ev)
{
    // TODO: make sure that tuning is successful or otherwise report an error
    SENF_LOG((senf::log::MESSAGE) ("DVB device tune event received: status=" << dvbController.status2String(ev.status))) ;
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::v_tune(DVBModulationParameter::fec_rate_t fec,
        DVBModulationParameter::modulation_t mod, DVBModulationParameter::guard_interval_t gi, unsigned frequency)
{
    dvbController.tuneDVB_T(
            frequency,
            INVERSION_AUTO,
            BANDWIDTH_8_MHZ,       // we assume a fixed bandwidth of 8 MHz
            fe_code_rate_t(fec),   // high priority stream code rate
            fe_code_rate_t(fec),   // low priority stream code rate
            fe_modulation_t(mod),  // modulation type
            TRANSMISSION_MODE_8K,
            fe_guard_interval_t(gi),
            HIERARCHY_NONE );
    HardwareDVBInterface::v_tune(fec, mod, gi, frequency);
}

prefix_ void senf::emu::HardwareDVBReceiverInterface::tune(std::string const & channelName)
{
    DVBConfigParser parser (FE_OFDM);
    struct dvb_frontend_parameters frontend (parser.getFrontendParam(
            parser.getConfigLine(channelName) ));
    tune(
            DVBModulationParameter::fec_rate_t(frontend.u.ofdm.code_rate_HP),
            DVBModulationParameter::modulation_t(frontend.u.ofdm.constellation),
            DVBModulationParameter::guard_interval_t(frontend.u.ofdm.guard_interval),
            frontend.frequency);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
