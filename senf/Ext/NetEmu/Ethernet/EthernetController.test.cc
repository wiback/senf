// $Id:$
//
// Copyright (C) 2011
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
