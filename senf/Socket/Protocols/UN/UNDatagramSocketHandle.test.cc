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
    \brief UNDatagramSocketHandle unit tests */

#include "UNDatagramSocketHandle.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(unDatagramSocketHandle)
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
