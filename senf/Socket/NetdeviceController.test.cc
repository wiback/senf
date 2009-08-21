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

/** \file
    \brief NetdeviceController unit tests */

//#include "NetdeviceController.test.hh"
//#include "NetdeviceController.test.ih"

// Custom includes
#include "NetdeviceController.hh"
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <net/if.h>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(NetdeviceController) {

    std::string ifname ("lo");
    senf::NetdeviceController ctrl (ifname);
    BOOST_CHECK_EQUAL( ctrl.interfaceName(), ifname);

    int index = ctrl.interfaceIndex();
    BOOST_CHECK_EQUAL( index, senf::NetdeviceController(index).interfaceIndex() );
    BOOST_CHECK_EQUAL( index, if_nametoindex( ifname.c_str()) );
    
    BOOST_CHECK_THROW( senf::NetdeviceController("invalid_interfacename"), senf::SystemException );
    
    int oldMTU (0);
    SENF_CHECK_NO_THROW( oldMTU = ctrl.mtu());
    
    bool promisc (false);
    SENF_CHECK_NO_THROW( promisc = ctrl.promisc());
    
    BOOST_CHECK( ctrl.isUp());

    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot run some tests of senf::NetdeviceController as non-root user");
        return;
    }

    SENF_CHECK_NO_THROW( ctrl.mtu(oldMTU-16));
    BOOST_CHECK_EQUAL( ctrl.mtu(), oldMTU-16);
    SENF_CHECK_NO_THROW( ctrl.mtu(oldMTU));
    BOOST_CHECK_EQUAL( ctrl.mtu(), oldMTU);
    
    SENF_CHECK_NO_THROW( ctrl.promisc( !promisc));
    BOOST_CHECK_EQUAL( ctrl.promisc(), !promisc);
    SENF_CHECK_NO_THROW( ctrl.promisc( promisc));
    BOOST_CHECK_EQUAL( ctrl.promisc(), promisc);
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
