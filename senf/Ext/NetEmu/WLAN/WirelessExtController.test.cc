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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief WirelessExtController unit tests */

#include "WirelessExtController.hh"

// Custom includes
#include <unistd.h>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(WirelessExtController)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::emu::WLANController as non-root user");

    char const * iface (getenv("SENF_WLAN_TEST_INTERFACE"));
    if (! iface) {
        BOOST_WARN_MESSAGE(false, "Set SENF_WLAN_TEST_INTERFACE to test the senf::emu::WLANController");
        return;
    }

    senf::emu::WirelessExtController wlc (iface);
    std::cout << "Frequency: " << wlc.frequency() << std::endl;
    std::cout << "SSID: " << wlc.ssid() << std::endl;
    std::cout << "CellId: " << wlc.cellId() << std::endl;
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
