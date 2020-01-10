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
