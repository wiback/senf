// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNDatagramSocketHandle unit tests */

//#include "UNDatagramSocketHandle.test.hh"
//#include "UNDatagramSocketHandle.test.ih"

// Custom includes
#include "UNDatagramSocketHandle.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(unDatagramSocketHandle)
{
    std::string testString ("Hallo Welt.");
    std::string socketPath (".socket-UNDatagramSocketHandle.test");

    BOOST_CHECKPOINT( 1 );
    senf::UNSocketAddress addr (socketPath);
    BOOST_CHECKPOINT( 2 );
    senf::UNDatagramClientSocketHandle inputSocket(addr);
    BOOST_CHECKPOINT( 3 );
    senf::UNDatagramClientSocketHandle outputSocket;


    BOOST_CHECKPOINT( 4 );
    outputSocket.writeto( addr, testString);

    BOOST_CHECKPOINT( 5 );
    BOOST_CHECK_EQUAL( inputSocket.read(), testString);




    BOOST_CHECKPOINT( 6 );
    outputSocket.close();
    BOOST_CHECKPOINT( 7 );
    inputSocket.close();

//    if (unlink(socketPath.c_str()) != 0)
//        perror( "unlink failed");
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
