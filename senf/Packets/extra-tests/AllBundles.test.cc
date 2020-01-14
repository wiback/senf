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
    \brief AllBundles unit tests */

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(packetLink)
{
    // This test shall test, if there is now duplicated packet registration in AllBundles.
    // The test will fail at the start with an assertion error in this case.

    boost::uint8_t data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,             // source mac
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x02,             // target mac
                              0x86, 0xdd,                                     // ethertype

                              0x60, 0x00, 0x00, 0x00,                         // IP version, class,
                                                                              //          flow label
                              0x00, 0x00,                                     // payload length
                              0x3B,                                           // no next header
                              0x10,                                           // hop limit
                              0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // source ip
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                              0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // target ip
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

    senf::EthernetPacket p (senf::EthernetPacket::create(data));

    BOOST_REQUIRE( p.next() );
    BOOST_CHECK( p.next().is<senf::IPv6Packet>() );
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
