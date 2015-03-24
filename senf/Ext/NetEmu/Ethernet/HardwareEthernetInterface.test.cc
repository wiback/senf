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
