// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

/** \file
    \brief InterfaceDecorator unit tests */

// Custom includes
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Utils/Console.hh>
#include "InterfaceId.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define CHECK_INTERFACEID_SIMPLE(ifaceId, nr)                               \
{                                                                           \
    senf::MACAddress id (ifaceId);                                          \
    BOOST_CHECK( id);                                                       \
    BOOST_CHECK( id.local());                                               \
    id[0] = 0;                                                              \
    BOOST_CHECK_EQUAL( id, senf::MACAddress(nr));                           \
}

SENF_AUTO_TEST_CASE(Simple_InterfaceIdFactory)
{
    senf::emu::SimpleInterfaceIdFactory factory;

    senf::MACAddress id1 (factory.getId());
    senf::MACAddress id2 (factory.getId());
    senf::MACAddress id3 (factory.getId());

    CHECK_INTERFACEID_SIMPLE( id1, 1 );
    CHECK_INTERFACEID_SIMPLE( id2, 2 );
    CHECK_INTERFACEID_SIMPLE( id3, 3 );

    factory.releaseId( id2);
    CHECK_INTERFACEID_SIMPLE( factory.getId(), 2 );

    factory.releaseId( id1);
    CHECK_INTERFACEID_SIMPLE( factory.getId(), 1 );

    factory.releaseId( id1);
    factory.releaseId( id2);
    factory.releaseId( id3);
    CHECK_INTERFACEID_SIMPLE( factory.getId(), 1 );
}

SENF_AUTO_TEST_CASE(InterfaceDeviceId)
{
    char const * ifname (getenv("SENF_TEST_INTERFACE"));
    if (! ifname) {
        BOOST_WARN_MESSAGE(false, "Set SENF_TEST_INTERFACE to test the senf::emu::InterfaceDeviceId");
        return;
    }
    senf::MACAddress mac (senf::NetdeviceController(ifname).hardwareAddress());
    std::cout << "InterfaceId for " << ifname << " ("
              << mac << "): " << senf::console::str(senf::emu::InterfaceDeviceId::get(mac)) << std::endl;
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
