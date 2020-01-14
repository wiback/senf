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


#include "EthernetController.hh"

// Custom includes
#include <net/if.h>
#include <senf/Utils/Exception.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(EthernetController)
{
    {
        std::string ifname ("lo");
        senf::emu::EthernetController ctrl (ifname);
        BOOST_CHECK_EQUAL( ctrl.interfaceName(), ifname);

        int index = ctrl.interfaceIndex();
        BOOST_CHECK_EQUAL( index, senf::emu::EthernetController(index).interfaceIndex() );
        BOOST_CHECK_EQUAL( index, if_nametoindex( ifname.c_str()) );

        BOOST_CHECK_THROW( senf::emu::EthernetController("invalid_interfacename"), senf::SystemException );
    }

    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test as EthernetController non-root user");

    char const * ifname (getenv("SENF_ETH_TEST_INTERFACE"));
    if (! ifname) {
        BOOST_WARN_MESSAGE(false, "Set SENF_ETH_TEST_INTERFACE to test the senf::emu::EthernetController");
        return;
    }

    senf::emu::EthernetController ctrl (ifname);
    BOOST_MESSAGE( "Speed of eth test interface: " << ctrl.speed());
    BOOST_MESSAGE( "Duplex mode of eth test interface: " << ctrl.duplex());
    BOOST_MESSAGE( "Link status eth test interface: " << ctrl.hasLink());
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
