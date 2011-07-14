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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief TunTapSocketHandle unit tests */

//#include "TunTapSocketHandle.test.hh"
//#include "TunTapSocketHandle.test.ih"

// Custom includes
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "TunTapSocketHandle.hh"
#include "PacketSocketHandle.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(tapSocketHandle)
{
    if (getuid() != 0) {
        BOOST_WARN_MESSAGE(false, "Cannot test senf::TunTapSocketHandle as non-root user");
        BOOST_CHECK( true );
        return;
    }

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
