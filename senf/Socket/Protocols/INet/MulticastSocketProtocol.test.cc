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
    \brief MulticastSocketProtocol unit tests */

// Custom includes
#include "UDPSocketHandle.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(multicastSocketProtocol)
{
    senf::UDPv4ClientSocketHandle sock;

    sock.protocol().broadcastEnabled(false);
    BOOST_CHECK( ! sock.protocol().broadcastEnabled() );
    sock.protocol().broadcastEnabled(true);
    BOOST_CHECK( sock.protocol().broadcastEnabled() );

    sock.protocol().mcTTL(4u);
    BOOST_CHECK_EQUAL( sock.protocol().mcTTL(), 4u );
    sock.protocol().mcTTL(1u);
    BOOST_CHECK_EQUAL( sock.protocol().mcTTL(), 1u );

    sock.protocol().mcLoop(false);
    BOOST_CHECK( ! sock.protocol().mcLoop() );
    sock.protocol().mcLoop(true);
    BOOST_CHECK( sock.protocol().mcLoop() );

    sock.protocol().mcIface("lo");

    SENF_CHECK_NO_THROW( sock.protocol().mcJoinSSMSource(
                             senf::INet4Address(0xE0000001u),
                             senf::INet4Address(0x7F000001u),
                             "lo") );
    // This fails with EADDRNOTAVAIL .. no idea why. I tried with 'eth' interface
    // and a real address (not loopback) to no avail.
//     SENF_CHECK_NO_THROW( sock.protocol().mcLeaveSSMSource(
//                              senf::INet4Address(0xE0000001u),
//                              senf::INet4Address(0x7F000001u),
//                              "lo") );

    senf::UDPv6ClientSocketHandle sock6;

    SENF_CHECK_NO_THROW( sock6.protocol().mcJoinSSMSource(
                             senf::INet6Address(0xFF00u, 0, 0, 0, 0, 0, 0, 1),
                             senf::INet6Address::Loopback,
                             "lo") );
    // This fails with EADDRNOTAVAIL .. no idea why. I tried with 'eth' interface
    // and a real address (not loopback) to no avail.
//     SENF_CHECK_NO_THROW( sock6.protocol().mcLeaveSSMSource(
//                              senf::INet6Address(0xFF00u, 0, 0, 0, 0, 0, 0, 1),
//                              senf::INet6Address::Loopback,
//                              "lo") );
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
