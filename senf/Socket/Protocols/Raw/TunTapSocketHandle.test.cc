// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
///////////////////////////////cc.p////////////////////////////////////////

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
