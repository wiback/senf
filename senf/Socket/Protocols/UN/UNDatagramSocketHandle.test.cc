// $Id$
//
// Copyright (C) 2007
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
//   David Wagner <dw6@berlios.de>
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
