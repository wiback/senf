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
    \brief TunTapSocketHandle unit tests */

#include "TunTapSocketHandle.hh"

// Custom includes
#include <unistd.h>
#include <stdlib.h>
#include "PacketSocketHandle.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(tapSocketHandle)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::TunTapSocketHandle as non-root user");

    senf::TapSocketHandle handle ("tap_unittest");
    int ret = system( "/sbin/ifconfig tap_unittest up");
    BOOST_CHECK_EQUAL( WEXITSTATUS(ret), 0);

    senf::PacketSocketHandle sock;
    SENF_CHECK_NO_THROW( sock.bind(senf::LLSocketAddress("tap_unittest")) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
