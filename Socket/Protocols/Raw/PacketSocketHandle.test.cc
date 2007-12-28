// $Id$
//
// Copyright (C) 2006
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

// Unit tests

//#include "PacketSocketHandle.test.hh"
//#include "PacketSocketHandle.test.ih"

#include "PacketSocketHandle.hh"

// Custom includes
#include <iostream>
#include <unistd.h>

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(packetSocketHandle)
{
    // We have a Problem here .. packet sockets are only allowed for root
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::PacketSocketHandle as non-root user");
        return;
    }

    {
        senf::PacketSocketHandle sock;

        BOOST_CHECK_NO_THROW( sock.bind(senf::LLSocketAddress("eth0")) );
        senf::LLSocketAddress a;
        BOOST_CHECK_NO_THROW( sock.local(a) );
        BOOST_CHECK_EQUAL( a.interface(), "eth0" );

        // How am I supposed to test read and write .. grmpf ..

        BOOST_CHECK_NO_THROW( sock.protocol().mcAdd(
                                  "eth0",senf::MACAddress::from_string("01-02-03-04-05-06")) );
        BOOST_CHECK_NO_THROW( sock.protocol().mcDrop(
                                  "eth0",senf::MACAddress::from_string("01-02-03-04-05-06")) );

        BOOST_CHECK_NO_THROW( sock.protocol().available() );
        BOOST_CHECK( ! sock.eof() );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
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
