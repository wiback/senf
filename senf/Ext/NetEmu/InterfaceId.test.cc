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
