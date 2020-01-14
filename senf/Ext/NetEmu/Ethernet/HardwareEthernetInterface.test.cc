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

#include "HardwareEthernetInterface.hh"

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>
#include <senf/Packets/DefaultBundle/UDPPacket.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(hardwareEthernetInterface)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test as non-root user");

    char const * ifname (getenv("SENF_ETH_TEST_INTERFACE"));
    if (! ifname) {
        BOOST_WARN_MESSAGE(
            false, "Set SENF_ETH_TEST_INTERFACE to test the senf::emu::HardwareEthernetInterface");
        BOOST_CHECK(true);
        return;
    }

    senf::emu::HardwareEthernetInterface iface (ifname);
    iface.enable();

    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;

    senf::ppi::connect(source, iface);
    senf::ppi::connect(iface, sink);

    senf::ppi::init();

    senf::EthernetPacket eth (senf::EthernetPacket::create());
    eth->destination() = senf::MACAddress(0xFFFFFFFFFFFFll);
    eth->source() = iface.id();
    senf::IPv6Packet ip (senf::IPv6Packet::createAfter(eth));
    ip->source() = senf::INet6Address::from_mac(eth->source());
    ip->destination() = senf::INet6Address::AllNodes;
    ip->hopLimit() = 32;
    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));
    senf::DataPacket::createAfter(udp, "PAYLOAD");
    eth.finalizeAll();

    source.submit(eth);
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
