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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief INet4Address unit tests */

//#include "INet4Address.test.hh"
//#include "INet4Address.test.ih"

// Custom includes
#include <arpa/inet.h>
#include <sstream>
#include "INet4Address.hh"
#include <senf/Utils/String.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(inet4Address)
{
    using senf::INet4Address;
    using senf::AddressSyntaxException;
    using senf::UnknownHostnameException;

    INet4Address addr (INet4Address::from_string("127.0.0.1"));
    BOOST_CHECK_EQUAL( addr, INet4Address::Loopback );
    SENF_CHECK_NOT_EQUAL( addr, INet4Address::Broadcast );

    addr = INet4Address::from_string("localhost");
    BOOST_CHECK_EQUAL( addr, INet4Address::Loopback );
    BOOST_CHECK( addr.loopback() );

    char data[] = { 128, 129, 130, 131 };
    addr = INet4Address::from_data(data);
    BOOST_CHECK_EQUAL( addr, INet4Address::from_string("128.129.130.131") );
    BOOST_CHECK_EQUAL( addr.inaddr(), htonl(0x80818283u) );
    BOOST_CHECK_EQUAL( addr.address(), 0x80818283u );

    BOOST_CHECK( ! addr.loopback() );
    BOOST_CHECK( ! addr.local() );
    BOOST_CHECK( INet4Address::from_string("192.168.1.2").local() );
    BOOST_CHECK( ! addr.multicast() );
    BOOST_CHECK( INet4Address::from_string("224.1.2.3").multicast() );
    BOOST_CHECK( ! addr.broadcast() );
    BOOST_CHECK( INet4Address::from_string("255.255.255.255").broadcast() );
    BOOST_CHECK( addr );
    BOOST_CHECK( ! INet4Address() );
    BOOST_CHECK_THROW( INet4Address::from_string(""), AddressSyntaxException );
    SENF_CHECK_NOT_EQUAL( INet4Address::from_string("www.go6.net"), INet4Address::None );
    BOOST_CHECK_THROW( INet4Address::from_string("invalid.host.fhg.de"), UnknownHostnameException);

    {
        std::stringstream str;
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str ("invalid.host.fhg.de");
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str;
        str << addr;
        BOOST_CHECK_EQUAL( str.str(), "128.129.130.131" );
        str >> addr;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(addr, INet4Address::from_string("128.129.130.131") );
    }
}

SENF_AUTO_UNIT_TEST(inet4Network)
{
    senf::INet4Network net (senf::INet4Address::Loopback,8);
    BOOST_CHECK_EQUAL( net.address().address(), 0x7F000000u );
    BOOST_CHECK_EQUAL( net.prefix_len(), 8u );
    BOOST_CHECK( net );
    BOOST_CHECK( ! senf::INet4Network() );

    senf::INet4Network net2 ("192.0.111.222/16");
    BOOST_CHECK_EQUAL( net2.address(), senf::INet4Address::from_string("192.0.0.0") );
    BOOST_CHECK_EQUAL( net2.prefix_len(), 16u );

    SENF_CHECK_NOT_EQUAL( net, net2 );
    BOOST_CHECK( net.match(senf::INet4Address::from_string("127.0.0.1")) );
    BOOST_CHECK( ! net2.match(senf::INet4Address::from_string("127.0.0.1")) );
    BOOST_CHECK( ! net.match(net2) );
    BOOST_CHECK( net2.match(senf::INet4Network("192.0.111.0/24")) );
    BOOST_CHECK( ! net2.match(senf::INet4Network("192.0.0.0/15")) );

    BOOST_CHECK_EQUAL( senf::str(net2), "192.0.0.0/16" );

    BOOST_CHECK_EQUAL( net2.host(-1), senf::INet4Address::from_string("192.0.255.255") );
    BOOST_CHECK_EQUAL( senf::str(net2.subnet(2u,24u)), "192.0.2.0/24" );

    BOOST_CHECK_THROW( senf::INet4Network(""), senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::INet4Network("192.0.2.0/24/beef"), senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::INet4Network("128.129.130.131/128"), senf::AddressSyntaxException );

    {
        std::stringstream str;
        senf::INet4Network net;
        str >> net;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str;
        senf::INet4Network net ("128.129.130.131/32");
        str << net;
        BOOST_CHECK_EQUAL( str.str(), "128.129.130.131/32");
        str >> net;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(net, senf::INet4Network("128.129.130.131/32"));
    }
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
