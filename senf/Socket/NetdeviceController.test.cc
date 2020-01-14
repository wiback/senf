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
    \brief NetdeviceController unit tests */

#include "NetdeviceController.hh"

// Custom includes
#include <net/if.h>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/IgnoreValue.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(NetdeviceController) {

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

    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot run some tests of senf::NetdeviceController as non-root user");

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
