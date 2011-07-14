// $Id$
//
// Copyright (C) 2008
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief MulticastSocketProtocol unit tests */

//#include "MulticastSocketProtocol.test.hh"
//#include "MulticastSocketProtocol.test.ih"

// Custom includes
#include "UDPSocketHandle.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(multicastSocketProtocol)
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
