// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief NetdeviceController unit tests */

//#include "NetdeviceController.test.hh"
//#include "NetdeviceController.test.ih"

// Custom includes
#include "NetdeviceController.hh"
#include <net/if.h>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/IgnoreValue.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(NetdeviceController) {

    std::string ifname ("lo");
    senf::NetdeviceController ctrl (ifname);
    BOOST_CHECK_EQUAL( ctrl.interfaceName(), ifname);

    int index = ctrl.interfaceIndex();
    BOOST_CHECK_EQUAL( index, senf::NetdeviceController(index).interfaceIndex() );
    BOOST_CHECK_EQUAL( index, if_nametoindex( ifname.c_str()) );

    BOOST_CHECK_THROW( senf::IGNORE(senf::NetdeviceController("invalid_interfacename")), senf::SystemException );

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
