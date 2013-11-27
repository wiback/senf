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
    \brief MACAddress unit tests */

//#include "MACAddress.test.hh"
//#include "MACAddress.test.ih"

// Custom includes
#include <sstream>
#include "MACAddress.hh"
#include <senf/Utils/String.hh>
#include "EUI64.hh"
#include <senf/Socket/Protocols/AddressExceptions.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(macAddress)
{
    using senf::MACAddress;
    using senf::AddressSyntaxException;

    std::string test ("A1-b2-C3:d4:E5:f6");
    MACAddress mac (MACAddress::from_string(test));

    BOOST_CHECK_EQUAL( mac[0], 0xA1u );
    BOOST_CHECK_EQUAL( mac[1], 0xB2u );
    BOOST_CHECK_EQUAL( mac[2], 0xC3u );
    BOOST_CHECK_EQUAL( mac[3], 0xD4u );
    BOOST_CHECK_EQUAL( mac[4], 0xE5u );
    BOOST_CHECK_EQUAL( mac[5], 0xF6u );

    {
        std::stringstream str;
        str >> mac;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str ( "invalid_mac_address");
        str >> mac;
        BOOST_CHECK( str.fail() );
    }
    {
        std::stringstream str;
        str << mac;
        BOOST_CHECK_EQUAL( str.str(), "a1:b2:c3:d4:e5:f6" );
        str >> mac;
        BOOST_CHECK( ! str.fail() );
    }

    BOOST_CHECK_EQUAL(mac, MACAddress::from_string(test));
    BOOST_CHECK( ! mac.local() );
    BOOST_CHECK( mac.multicast() );
    BOOST_CHECK( ! mac.broadcast() );
    BOOST_CHECK( mac );
    BOOST_CHECK_EQUAL( mac.oui(), 0xa1b2c3u );
    BOOST_CHECK_EQUAL( mac.nic(), 0xd4e5f6u );
    BOOST_CHECK_EQUAL( mac.eui64(), 0xa1b2c3fffed4e5f6llu );

    MACAddress mac2;
    BOOST_CHECK( ! mac2 );
    mac2 = MACAddress::from_string("ff:ff:ff:ff:ff:ff");
    BOOST_CHECK( mac2.broadcast() );
    BOOST_CHECK_EQUAL( mac2, MACAddress::Broadcast );
    char data[] = { 0x01,0x02,0x03,0x04,0x05,0x06 };
    mac2 = MACAddress::from_data(data);
    BOOST_CHECK_EQUAL( senf::str(mac2), "01:02:03:04:05:06" );
    SENF_CHECK_NOT_EQUAL( mac, mac2 );
    mac2 = mac;
    BOOST_CHECK( mac == mac2 );
    BOOST_CHECK_EQUAL( senf::str(mac2), "a1:b2:c3:d4:e5:f6" );

    BOOST_CHECK_THROW( MACAddress::from_string("123:2:3:4:5:6"), AddressSyntaxException );
    BOOST_CHECK_THROW( MACAddress::from_string("01:02:03:04:05"), AddressSyntaxException );
    // test all invalid ascii ranges
    BOOST_CHECK_THROW( MACAddress::from_string("01:02:03:04:05:z6"), AddressSyntaxException );
    BOOST_CHECK_THROW( MACAddress::from_string("01:02:03:04:05:/6"), AddressSyntaxException );
    BOOST_CHECK_THROW( MACAddress::from_string("01:02:03:04:05:@6"), AddressSyntaxException );
    BOOST_CHECK_THROW( MACAddress::from_string("01:02:03:04:05:[6"), AddressSyntaxException );

    BOOST_CHECK_EQUAL( mac, MACAddress::from_eui64(senf::EUI64(0xa1b2c3fffed4e5f6llu)) );
    BOOST_CHECK_THROW( MACAddress::from_eui64(senf::EUI64(0ull)),
                       AddressSyntaxException );

    BOOST_CHECK_EQUAL( MACAddress(0x1a2b3c4d5e6fULL).uint64(), 0x1a2b3c4d5e6fULL);

    BOOST_CHECK_EQUAL( mac, senf::EUI64::from_mac(mac) );
    BOOST_CHECK_EQUAL( senf::EUI64::from_mac(mac), mac );
}

#ifdef SENF_CXX11_ENABLED
#include <unordered_map>

SENF_AUTO_UNIT_TEST(macAddress_std_hash)
{
    std::unordered_map<senf::MACAddress, int> map;
    map.insert( std::make_pair(senf::MACAddress::Broadcast, 42));
}

#endif

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
