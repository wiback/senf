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
    \brief HardwareWLANInterface unit tests */

#include "HardwareWLANInterface.hh"

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

#define MHZ_TO_KHZ(freq) ((freq) * 1000)

SENF_AUTO_TEST_CASE(hardwareWLANInterface)
{
//    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::emu::HardwareWLANInterface as non-root user");
//
//    char const * ifname (getenv("SENF_WLAN_TEST_INTERFACE"));
//    if (! ifname) {
//        BOOST_WARN_MESSAGE(false, "Set SENF_WLAN_TEST_INTERFACE to test the senf::emu::HardwareWLANInterface");
//        return;
//    }

    emu::HardwareWLANInterface iface (std::make_pair("wlan0", "mon0"));
    iface.frequency(iface.frequencies().begin()->frequencyLower, MHZ_TO_KHZ(20));
    iface.disable();
    iface.enable();

    ppi::module::debug::ActiveSource source;
    ppi::module::debug::PassiveSink sink;

    ppi::connect(source, iface);
    ppi::connect(iface, sink);

    ppi::init();

    EthernetPacket eth (EthernetPacket::create());
    eth->destination() = MACAddress::Broadcast;
    eth->source() = iface.id();
    IPv6Packet ip (IPv6Packet::createAfter(eth));
    ip->source() = INet6Address::from_mac(eth->source());
    ip->destination() = INet6Address::AllNodes;
    ip->hopLimit() = 32;
    UDPPacket udp (UDPPacket::createAfter(ip));
    DataPacket::createAfter(udp, "PAYLOAD");
    eth.finalizeAll();

    source.submit(eth);

    unsigned i (0);
    std::cout << "Registered frequencies: " << std::endl;
    for (emu::WirelessInterface::Frequency const & freq : iface.frequencies()) {
        std::cout << "  "  << freq.frequencyLower << "," << freq.bandwidthLower;
        if ((++i % 5)==0) std::cout << std::endl;
    }
    std::cout << std::endl;

    try {
        iface.leaveAdhocNetwork();
    } catch (...) {}

    NetdeviceController(ifname).down();
    iface.joinAdhocNetwork("test", iface.frequencies().begin()->frequencyLower, MHZ_TO_KHZ(20))
        ->bssid(senf::MACAddress::None);

//    emu::WirelessNLController wnlc (ifname);
//    emu::BitrateParameters const & rates (wnlc.bitrates());
//    unsigned rate = 0;
//    if (! rates.legacy_24.empty())
//        rate = *rates.legacy_24.begin();
//    if (! rates.legacy_5.empty())
//        rate = *rates.legacy_5.begin();
//    if (rate)
//        iface.modulation(emu::WLANModulationParameter::Legacy, rate);
}

SENF_AUTO_TEST_CASE(hardwareWLANInterface_monitor)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot run tests as non-root user");

    char const * ifname (getenv("SENF_WLAN_TEST_INTERFACE"));
    if (! ifname) {
        return;
    }
    char const * monIfname (getenv("SENF_WLAN_TEST_MON_INTERFACE"));
    if (! monIfname) {
        BOOST_WARN_MESSAGE(false, "Set SENF_WLAN_TEST_MON_INTERFACE to test the monitor "
                "modus of emu::HardwareWLANInterface");
        return;
    }

    emu::HardwareWLANInterface iface (ifname);
    iface.frequency(iface.frequencies().begin()->frequencyLower);
    iface.monitorDevice( monIfname);
    iface.disable();
    iface.enable();
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
