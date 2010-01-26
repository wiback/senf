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
    \brief BSDSocketAddress unit tests */

//#include "BSDSocketAddress.test.hh"
//#include "BSDSocketAddress.test.ih"

// Custom includes
#include "BSDSocketAddress.hh"
#include <senf/Socket/Protocols/INet/INetAddressing.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(bsdSocketAddress)
{
    senf::GenericBSDSocketAddress g1;
    senf::GenericBSDSocketAddress g2 (senf::INet4SocketAddress("1.2.3.4:5678"));

    BOOST_CHECK_EQUAL( g2.family(), senf::INet4SocketAddress::addressFamily+0 );
    BOOST_CHECK_EQUAL( senf::sockaddr_cast<senf::INet4SocketAddress>(g2).port(), 5678u );
    BOOST_CHECK_THROW( senf::sockaddr_cast<senf::INet6SocketAddress>(g2), std::bad_cast );
    BOOST_CHECK( g2 );
    BOOST_CHECK( ! g1 );
    BOOST_CHECK( g1 != g2 );
    g1 = g2;
    BOOST_CHECK( g1 == g2 );
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
