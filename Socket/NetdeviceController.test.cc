// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// Unit tests

//#include "NetdeviceController.test.hh"
//#include "NetdeviceController.test.ih"

// Custom includes
#include "NetdeviceController.hh"
#include "Protocols/Raw/MACAddress.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(NetdeviceController) {

    senf::NetdeviceController ctrl ("wlan0");
    std::cout << "name: " << ctrl.interfaceName() << "\n";

    senf::MACAddress oldAddr(ctrl.hardwareAddress());
    int oldMTU = ctrl.mtu();

    std::cout << "hw addr: " << oldAddr << "\n";
    std::cout << "mtu: " << oldMTU << "\n";

    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot run some tests of senf::NetdeviceController as non-root user");
        return;
    }

    ctrl.mtu(oldMTU - 16);
    std::cout << "new mtu: " << ctrl.mtu() << "\n";
    ctrl.mtu(oldMTU);

    senf::MACAddress newAddr(senf::MACAddress::from_string("00:18:de:2e:ec:00"));
    ctrl.hardwareAddress(newAddr);
    std::cout << "new hw addr: " << ctrl.hardwareAddress() << "\n";
    ctrl.hardwareAddress(oldAddr);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
