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
    \brief MulticastSocketProtocol.test unit tests */

//#include "MulticastSocketProtocol.test.hh"
//#include "MulticastSocketProtocol.test.ih"

// Custom includes
#include "UDPSocketHandle.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(multicastSocketProtocol)
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
}

///////////////////////////////cc.e////////////////////////////////////////
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
