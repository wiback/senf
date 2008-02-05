// $Id$
//
// Copyright (C) 2007
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
    \brief INet4Address.test unit tests */

//#include "INet4Address.test.hh"
//#include "INet4Address.test.ih"

// Custom includes
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include "INet4Address.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet4Address)
{
    senf::INet4Address addr (senf::INet4Address::from_string("127.0.0.1"));
    BOOST_CHECK_EQUAL( addr, senf::INet4Address::Loopback );
    BOOST_CHECK( addr != senf::INet4Address::Broadcast );

    addr = senf::INet4Address::from_string("localhost");
    BOOST_CHECK_EQUAL( addr, senf::INet4Address::Loopback );
    BOOST_CHECK( addr.loopback() );

    char data[] = { 128, 129, 130, 131 };
    addr = senf::INet4Address::from_data(data);
    BOOST_CHECK_EQUAL( addr, senf::INet4Address::from_string("128.129.130.131") );
    BOOST_CHECK_EQUAL( addr.inaddr(), htonl(0x80818283u) );
    BOOST_CHECK_EQUAL( addr.address(), 0x80818283u );

    BOOST_CHECK( ! addr.loopback() );
    BOOST_CHECK( ! addr.local() );
    BOOST_CHECK( senf::INet4Address::from_string("192.168.1.2").local() );
    BOOST_CHECK( ! addr.multicast() );
    BOOST_CHECK( senf::INet4Address::from_string("224.1.2.3").multicast() );
    BOOST_CHECK( ! addr.broadcast() );
    BOOST_CHECK( senf::INet4Address::from_string("255.255.255.255").broadcast() );
    BOOST_CHECK( addr );
    BOOST_CHECK( ! senf::INet4Address() );

    std::stringstream str;
    str << addr;
    BOOST_CHECK_EQUAL( str.str(), "128.129.130.131" );
}

BOOST_AUTO_UNIT_TEST(inet4Network)
{
    senf::INet4Network net (senf::INet4Address::Loopback,8);
    BOOST_CHECK_EQUAL( net.address().address(), 0x7F000000u );
    BOOST_CHECK_EQUAL( net.prefix_len(), 8u );
    BOOST_CHECK( net );
    BOOST_CHECK( ! senf::INet4Network() );
    
    senf::INet4Network net2 ("192.0.111.222/16");
    BOOST_CHECK_EQUAL( net2.address(), senf::INet4Address::from_string("192.0.0.0") );
    BOOST_CHECK_EQUAL( net2.prefix_len(), 16u );

    BOOST_CHECK( net != net2 );
    BOOST_CHECK( net.match(senf::INet4Address::from_string("127.0.0.1")) );
    BOOST_CHECK( ! net2.match(senf::INet4Address::from_string("127.0.0.1")) );
    BOOST_CHECK( ! net.match(net2) );
    BOOST_CHECK( net2.match(senf::INet4Network("192.0.111.0/24")) );
    BOOST_CHECK( ! net2.match(senf::INet4Network("192.0.0.0/15")) );

    BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(net2), "192.0.0.0/16" );

    BOOST_CHECK_EQUAL( net2.host(-1), senf::INet4Address::from_string("192.0.255.255") );
    BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(net2.subnet(2u,24u)), "192.0.2.0/24" );
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
