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
