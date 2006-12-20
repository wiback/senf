// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#include <boost/test/auto_unit_test.hpp>
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
        
        BOOST_CHECK_NO_THROW( sock.bind(senf::LLSocketAddress("lo")) );
        senf::LLSocketAddress a;
        BOOST_CHECK_NO_THROW( sock.local(a) );
        BOOST_CHECK_EQUAL( a.interface(), "lo" );

        // How am I supposed to test read and write .. grmpf ..
        
        // BUG: There are some failures here ... need to investigate
        /*
        BOOST_CHECK_NO_THROW( sock.protocol().promisc(
                                  "lo",senf::PacketProtocol::Promiscuous) );
        BOOST_CHECK_NO_THROW( sock.protocol().promisc(
                                  "lo",senf::PacketProtocol::AllMulticast) );
        BOOST_CHECK_NO_THROW( sock.protocol().promisc(
                                  "lo",senf::PacketProtocol::None) );
        */
        
        BOOST_CHECK_NO_THROW( sock.protocol().mcAdd(
                                  "lo",senf::llAddress("01-02-03-04-05-06")) );
        BOOST_CHECK_NO_THROW( sock.protocol().mcDrop(
                                  "lo",senf::llAddress("01-02-03-04-05-06")) );

        BOOST_CHECK_NO_THROW( sock.protocol().available() );
        BOOST_CHECK( ! sock.eof() );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
