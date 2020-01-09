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
    \brief EmulatedDVBInterface non-inline non-template implementation */

#include "EmulatedDVBInterface.hh"
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedDVBInterface

prefix_ senf::emu::EmulatedDVBInterface::EmulatedDVBInterface()
    : modulationId_(0), dev_("NetEMU-DVB")
{
    // register all known modulations
    for (ModulationParameter::id_t id : DVBModulationParameterRegistry::instance().parameterIds()) {
        registerModulation( id);
    }
}

prefix_ std::string const & senf::emu::EmulatedDVBInterface::v_device()
    const
{
    return dev_;
}

prefix_ void senf::emu::EmulatedDVBInterface::init()
{
    EmulatedWirelessInterface::init();
    DVBInterface::init();

    namespace fty = console::factory;
    consoleDir().remove("registerFrequency");
    consoleDir().add("registerFrequency",
            fty::Command(SENF_MEMBINDFNP(void, EmulatedDVBInterface, registerFrequency, (unsigned)))
        .arg("frequency","channel frequency in kHz")
        .doc("Register valid frequency") );
    consoleDir().add("registerFrequency",
            fty::Command(SENF_MEMBINDFNP(void, EmulatedDVBInterface, registerFrequency, (unsigned, unsigned)))
        .arg("frequencyLower", "lower bound channel frequency in kHz")
        .arg("frequencyUpper", "upper bound channel frequency in kHz")
        .doc("Register valid frequencies") );
}

prefix_ void senf::emu::EmulatedDVBInterface::v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
        DVBModulationParameter::guard_interval_t gi, unsigned freq)
{
    ModulationParameter::id_t modId (DVBModulationParameterRegistry::instance().parameterId(fec, mod, gi));
    DVBModulationParameterRegistry::instance().findModulationById(modId); // will throw if id is unknown
    modulationId_ = modId;
    frequency(freq, 8000); // we assume a fixed bandwidth of 8 MHz
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedDVBSenderInterfaceNet

prefix_ senf::emu::detail::EmulatedDVBSenderInterfaceNet::
EmulatedDVBSenderInterfaceNet()
    : transmitFilter_ (self()),
      transmitInput (transmitFilter_.input),
      transmitOutput (transmitFilter_.output)
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedDVBReceiverInterfaceNet

prefix_ senf::emu::detail::EmulatedDVBReceiverInterfaceNet::
EmulatedDVBReceiverInterfaceNet()
    : receiveFilter_(self()),
      receiveInput (receiveFilter_.input),
      receiveOutput (receiveFilter_.output)
{}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedDVBSenderInterface

namespace {
    // Arbitrary magic number used to identify the packet type.
    boost::uint32_t const dvbMagic (0x44564254u);
}

prefix_ senf::emu::EmulatedDVBSenderInterface::EmulatedDVBSenderInterface()
    : WirelessTransmitter(EmulatedDVBSenderInterfaceNet::transmitInput),
      power_ (0)
{
    ppi::connect(EmulatedDVBSenderInterfaceNet::transmitOutput, transmitterJack);

    disable();
    id( uniqueMAC(dvbMagic));

    EmulatedDVBInterface::init();
    WirelessTransmitter::init();
    EmulatedWirelessTransmitter::init();
//    EmulatedWirelessTransmitter::delay(ClockService::milliseconds(20));
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::EmulatedDVBSenderInterface::v_modulationId()
    const
{
    return modulationId_;
}

prefix_ void senf::emu::EmulatedDVBSenderInterface::v_modulationId(ModulationParameter::id_t id)
{
    DVBModulationParameterRegistry::instance().findModulationById( id);  // will throw if id is unknown
    modulationId_ = id;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedDVBReceiverInterface

prefix_ senf::emu::EmulatedDVBReceiverInterface::EmulatedDVBReceiverInterface()
    : WirelessReceiver(EmulatedDVBReceiverInterfaceNet::receiveOutput)
{
    ppi::connect(receiverJack, EmulatedDVBReceiverInterfaceNet::receiveInput);

    disable();
    id( uniqueMAC(dvbMagic));

    EmulatedDVBInterface::init();
    WirelessReceiver::init();
    EmulatedWirelessReceiver::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedDVBReceiverReceiveFilter

prefix_ senf::emu::detail::EmulatedDVBReceiverReceiveFilter::
EmulatedDVBReceiverReceiveFilter(EmulatedDVBReceiverInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    input.onRequest( &EmulatedDVBReceiverReceiveFilter::request);
}

prefix_ void senf::emu::detail::EmulatedDVBReceiverReceiveFilter::request()
{
    DVBPacketHeader p;
    try {
        // in case an emulated WLAN device is tuned on the same frequency this
        // will cause an exception
        p = input();
    }
    catch (senf::WrapException<std::bad_cast> &) {
        SENF_LOG( (senf::log::CRITICAL) ("ignoring non-emulated DVB frame"));
        return;
    }

    EthernetPacket e (p.find<EthernetPacket>(senf::nothrow));
    if (! e)
        return;
    if (iface_.annotationMode())
        output(prependAnnotationsPacket(e));
    else
        output(e);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedDVBSenderTransmitFilter

prefix_ senf::emu::detail::EmulatedDVBSenderTransmitFilter::
EmulatedDVBSenderTransmitFilter(EmulatedDVBSenderInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    input.onRequest(&EmulatedDVBSenderTransmitFilter::request);
}

prefix_ void senf::emu::detail::EmulatedDVBSenderTransmitFilter::request()
{
    EthernetPacket e (input());
    e->source() = iface_.id();
    DVBPacketHeader dph (DVBPacketHeader::createBefore(e));
    dph.finalizeThis();
    output(dph);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_



// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
