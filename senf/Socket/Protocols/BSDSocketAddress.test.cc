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
    \brief BSDSocketAddress unit tests */

#include "BSDSocketAddress.hh"

// Custom includes
#include <senf/Socket/Protocols/INet/INetAddressing.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(bsdSocketAddress)
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
