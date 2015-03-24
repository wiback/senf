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
    \brief InterfaceAPIBase non-inline non-template implementation */

#include "InterfaceAPIBase.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,Bidirectional,void >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::Bidirectional>::
init()
{
    WiredInterface::init();
    WiredReceiver::init();
    WiredTransmitter::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,Bidirectional,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::Bidirectional,
                            senf::emu::interface::Hardware>::
init()
{
    BidirectionalWiredInterface::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,ReceiveOnly,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Hardware>::
init()
{
    WiredInterface::init();
    WiredReceiver::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,TransmitOnly,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Hardware>::
init()
{
    WiredInterface::init();
    WiredTransmitter::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,Bidirectional,void >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wireless,
                            senf::emu::interface::Bidirectional,
                            void>::
init()
{
    WirelessInterface::init();
    WirelessReceiver::init();
    WirelessTransmitter::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,Bidirectional,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wireless,
                            senf::emu::interface::Bidirectional,
                            senf::emu::interface::Hardware>::
init()
{
    BidirectionalWirelessInterface::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,ReceiveOnly,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wireless,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Hardware>::
init()
{
    WirelessInterface::init();
    WirelessReceiver::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,TransmitOnly,Hardware >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wireless,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Hardware>::
init()
{
    WirelessInterface::init();
    WirelessTransmitter::init();
    HardwareInterface::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,Bidirectional,Emulated >


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,ReceiveOnly,Emulated >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_enable()
{
    enableEmulatedInterface();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_disable()
{
    disableEmulatedInterface();
}

prefix_ bool
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_enabled()
    const
{
    return emulatedInterfaceEnabled();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_initEmulationInterface()
{
    initEmulatedReceiver();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_deinitEmulationInterface()
{
    deinitEmulatedReceiver();
}

prefix_ bool
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_promisc()
    const
{
    return promiscEmulatedReceiver();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_promisc(bool v)
{
    promiscEmulatedReceiver(v);
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_mcAdd(senf::MACAddress const & address)
{
    mcAddEmulatedReceiver(address);
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_mcDrop(senf::MACAddress const & address)
{
    mcDropEmulatedReceiver(address);
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
init()
{
    WiredInterface::init();
    WiredReceiver::init();
    EmulatedWiredInterface::init();
    EmulatedWiredReceiver::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wired,TransmitOnly,Emulated >

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_enable()
{
    enableEmulatedInterface();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_disable()
{
    disableEmulatedInterface();
}

prefix_ bool
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_enabled()
    const
{
    return emulatedInterfaceEnabled();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_initEmulationInterface()
{
    initEmulatedTransmitter();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_deinitEmulationInterface()
{
    deinitEmulatedTransmitter();
}

prefix_ unsigned
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_mtu()
    const
{
    return mtuEmulatedTransmitter();
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
v_mtu(unsigned v)
{
    mtuEmulatedTransmitter(v);
}

prefix_ void
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::TransmitOnly,
                            senf::emu::interface::Emulated>::
init()
{
    WiredInterface::init();
    WiredTransmitter::init();
    EmulatedWiredInterface::init();
    EmulatedWiredTransmitter::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,Bidirectional,Emulated >


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,ReceiveOnly,Emulated >


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceAPIBase< Wireless,TransmitOnly,Emulated >


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
