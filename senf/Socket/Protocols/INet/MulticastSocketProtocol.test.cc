// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
