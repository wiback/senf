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
    \brief MACAddress.test unit tests */

//#include "MACAddress.test.hh"
//#include "MACAddress.test.ih"

// Custom includes
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "MACAddress.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(macAddress)
{
	std::string test ("A1-b2-C3:d4:E5:f6");
    senf::MACAddress mac (senf::MACAddress::from_string(test));

    BOOST_CHECK_EQUAL( mac[0], 0xA1u );
    BOOST_CHECK_EQUAL( mac[1], 0xB2u );
    BOOST_CHECK_EQUAL( mac[2], 0xC3u );
    BOOST_CHECK_EQUAL( mac[3], 0xD4u );
    BOOST_CHECK_EQUAL( mac[4], 0xE5u );
    BOOST_CHECK_EQUAL( mac[5], 0xF6u );

    std::stringstream str;
    str << mac;
    BOOST_CHECK_EQUAL( str.str(), "a1:b2:c3:d4:e5:f6" );
    BOOST_CHECK_EQUAL (mac.toString(), "a1:b2:c3:d4:e5:f6");
    
    BOOST_CHECK( ! mac.local() );
    BOOST_CHECK( mac.multicast() );
    BOOST_CHECK( ! mac.broadcast() );
    BOOST_CHECK( mac );
    BOOST_CHECK_EQUAL( mac.oui(), 0xa1b2c3u );
    BOOST_CHECK_EQUAL( mac.nic(), 0xd4e5f6u );
    BOOST_CHECK_EQUAL( mac.eui64(), 0xa1b2c3fffed4e5f6llu );

    senf::MACAddress mac2;
    BOOST_CHECK( ! mac2 );
    mac2 = senf::MACAddress::from_string("ff:ff:ff:ff:ff:ff");
    BOOST_CHECK( mac2.broadcast() );
    BOOST_CHECK_EQUAL( mac2, senf::MACAddress::Broadcast );
    char data[] = { 0x01,0x02,0x03,0x04,0x05,0x06 };
    mac2 = senf::MACAddress::from_data(data);
    BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(mac2), "01:02:03:04:05:06" );
    BOOST_CHECK( mac != mac2 );
    mac2 = mac;
    BOOST_CHECK( mac == mac2 );
    BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(mac2), "a1:b2:c3:d4:e5:f6" );

    BOOST_CHECK_THROW( senf::MACAddress::from_string("1:2:3:4:5:6"), 
                       senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::MACAddress::from_string("01:02:03:04:05"), 
                       senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::MACAddress::from_string("01:02:03:04:05:z6"), 
                       senf::AddressSyntaxException );

    BOOST_CHECK_EQUAL( mac, senf::MACAddress::from_eui64(0xa1b2c3fffed4e5f6llu) );
    BOOST_CHECK_THROW( senf::MACAddress::from_eui64(0u), senf::AddressSyntaxException );
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
