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
v_annotationMode(bool a)
{
    annotationModeEmulatedReceiver(a);
}

prefix_ bool
senf::emu::InterfaceAPIBase<senf::emu::interface::Wired,
                            senf::emu::interface::ReceiveOnly,
                            senf::emu::interface::Emulated>::
v_annotationMode()
    const
{
    return annotationModeEmulatedReceiver();
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
