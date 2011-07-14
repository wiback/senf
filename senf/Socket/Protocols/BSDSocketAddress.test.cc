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
//   Stefan Bund <g0dil@berlios.de>

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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(bsdSocketAddress)
{
    senf::GenericBSDSocketAddress g1;
    senf::GenericBSDSocketAddress g2 (senf::INet4SocketAddress("1.2.3.4:5678"));

    BOOST_CHECK_EQUAL( g2.family(), senf::INet4SocketAddress::addressFamily+0 );
    BOOST_CHECK_EQUAL( senf::sockaddr_cast<senf::INet4SocketAddress>(g2).port(), 5678u );
    BOOST_CHECK_THROW( senf::sockaddr_cast<senf::INet6SocketAddress>(g2), std::bad_cast );
    BOOST_CHECK( g2 );
    BOOST_CHECK( ! g1 );
    SENF_CHECK_NOT_EQUAL( g1, g2 );
    g1 = g2;
    BOOST_CHECK( g1 == g2 );
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
