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
    \brief EmulatedEthernetInterface non-inline non-template implementation */

#include "EmulatedEthernetInterface.hh"
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedEthernetInterfaceNet

prefix_ senf::emu::detail::EmulatedEthernetInterfaceNet::
EmulatedEthernetInterfaceNet()
    : receiveFilter_ (self()), transmitFilter_ (self()),
      receiveInput (receiveFilter_.input),
      receiveOutput (receiveFilter_.output), transmitInput (transmitFilter_.input),
      transmitOutput (transmitFilter_.output)
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedEthernetInterface

namespace {
    // Arbitrary magic number used to identify the emulation header for Ethernet packets
    // Defined to be the 32bit integer representation of the string 'eth' in network byte order
    boost::uint32_t const ethernetMagic (0x657468);
}

prefix_ senf::emu::EmulatedEthernetInterface::EmulatedEthernetInterface()
    : Base(receiveOutput, transmitInput), vlanGroupRange_ ("239.203.0.0/16"),
      portbase_(11264), vlanId_(0), dev_("NetEMU-Ethernet")
{
    ppi::connect(receiverJack, receiveInput);
    ppi::connect(transmitOutput, transmitterJack);

    disable();
    id( uniqueMAC(ethernetMagic));
    mtu(1500u);

    init();
}

prefix_ void senf::emu::EmulatedEthernetInterface::init()
{
    Base::init();

    namespace kw = console::kw;
    namespace fty = console::factory;
    consoleDir()
        .add("vlan",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedEthernetInterface,
                                          vlan, (boost::uint16_t) ))
             .arg("vlan", "VLAN id")
             .doc("Get/set the VLAN id of the ethernet interface")
             );
    consoleDir()
        .add("vlan",
             fty::Command(SENF_MEMBINDFNP(boost::uint16_t, EmulatedEthernetInterface,
                                          vlan, () const))
             );

    UDPClientHandle::Address addr (vlanGroupRange_.host(vlanId_),portbase_+vlanId_);
    emulationAddress(addr);
}

prefix_ void senf::emu::EmulatedEthernetInterface::v_id(MACAddress const & eui)
{
    id_ = eui;
}

prefix_ senf::MACAddress senf::emu::EmulatedEthernetInterface::v_id()
    const
{
    return id_;
}

prefix_ boost::uint16_t senf::emu::EmulatedEthernetInterface::vlan()
    const
{
    return vlanId_;
}

prefix_ void senf::emu::EmulatedEthernetInterface::vlan(boost::uint16_t id)
{
    vlanId_ = id;
    UDPClientHandle::Address addr (vlanGroupRange_.host(vlanId_),portbase_+vlanId_);
    emulationAddress(addr);
}

prefix_ std::string const & senf::emu::EmulatedEthernetInterface::v_device()
    const
{
    return dev_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedEthernetReceiveFilter

prefix_ senf::emu::detail::EmulatedEthernetReceiveFilter::
EmulatedEthernetReceiveFilter(EmulatedEthernetInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    input.onRequest(&EmulatedEthernetReceiveFilter::request);
}

prefix_ void senf::emu::detail::EmulatedEthernetReceiveFilter::request()
{
    EthernetPacketHeader p (input());
    EthernetPacket e (p.find<EthernetPacket>(senf::nothrow));
    if (e) {
        e.annotation<annotations::Interface>().value = iface_.id();
        {
            annotations::Quality & q (e.annotation<annotations::Quality>());
            q.rssi = 127;            // for now, we report the maximum signal 'quality'
            q.snr = q.rssi - (-128); // 'default' noise floor to ensure that we always report a higher SNR compared to wireless
            q.flags.frameLength = e.size();
        }

        if (iface_.annotationMode())
            output(prependAnnotationsPacket(e));
        else
            output(e);
    }
    return;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedEthernetTransmitFilter

prefix_ senf::emu::detail::EmulatedEthernetTransmitFilter::
EmulatedEthernetTransmitFilter(EmulatedEthernetInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    input.onRequest(&EmulatedEthernetTransmitFilter::request);
}

prefix_ void senf::emu::detail::EmulatedEthernetTransmitFilter::request()
{
    EthernetPacket e (input());
    e->source() = iface_.id();
    EthernetPacketHeader eph (EthernetPacketHeader::createBefore(e));
    eph.finalizeThis();
    output(eph);
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
