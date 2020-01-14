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
