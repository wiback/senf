// $Id$
//
// Copyright (C) 2009
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
    \brief EUI64.test unit tests */

//#include "EUI64.test.hh"
//#include "EUI64.test.ih"

// Custom includes
#include "EUI64.hh"
#include <senf/Socket/Protocols/AddressExceptions.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(eui64)
{
    senf::EUI64 eui;
    BOOST_CHECK( !eui );
    BOOST_CHECK( !eui.isMACCompatible() );

    eui = senf::EUI64::from_mac(senf::MACAddress(0x102030405060ull));
    boost::uint8_t data[] = { 0x10u, 0x20u, 0x30u, 0xffu, 0xfeu, 0x40u, 0x50u, 0x60u };
    BOOST_CHECK_EQUAL_COLLECTIONS( eui.begin(), eui.end(), data, data+sizeof(data)/sizeof(data[0]) );
    BOOST_CHECK( eui );
    BOOST_CHECK( eui.isMACCompatible() );
    BOOST_CHECK( ! eui.local() );
    BOOST_CHECK( ! eui.group() );
    BOOST_CHECK_EQUAL( eui.uint64(), 0x102030fffe405060ull );

    BOOST_CHECK_EQUAL( eui, senf::EUI64::from_data(data) );
    BOOST_CHECK_EQUAL( eui, senf::EUI64::from_string("10:20:30:ff-FE:40:50:60") );

    BOOST_CHECK_THROW( senf::EUI64::from_string("123:20:30:40:50:60:70:80"),
                       senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::EUI64::from_string("12:20:30:40:50:60:70"),
                       senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::EUI64::from_string("12:20:30:40:50:60:70:8g"),
                       senf::AddressSyntaxException );
    BOOST_CHECK_THROW( senf::EUI64::from_string("12:20:30:40:50:60:70:80:90"),
                       senf::AddressSyntaxException );

    BOOST_CHECK_EQUAL( senf::EUI64::None, senf::EUI64(0) );
    BOOST_CHECK(! senf::EUI64::None );

    {
        std::stringstream ss;
        ss << std::uppercase << eui;
        BOOST_CHECK_EQUAL( ss.str(), "10:20:30:FF-FE:40:50:60" );
        eui = senf::EUI64();
        BOOST_CHECK( !eui );
        ss >> eui;
        BOOST_CHECK_EQUAL( eui, senf::EUI64(0x102030fffe405060ull) );

        BOOST_CHECK( (ss >> eui).fail() );
    }

    {
        std::stringstream ss;
        ss << "01:02:03:04-05:06:07:108";
        BOOST_CHECK( (ss >> eui).fail() );
    }
}

SENF_AUTO_UNIT_TEST(eui64_fillRight)
{
    std::string eui64Str ("01:02:03:ff-fe:04:05:06");
    senf::EUI64 eui64 (senf::EUI64::from_string( eui64Str));

    std::stringstream ss;
    ss << std::left << eui64;
    BOOST_CHECK_EQUAL( ss.str(), eui64Str);
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
