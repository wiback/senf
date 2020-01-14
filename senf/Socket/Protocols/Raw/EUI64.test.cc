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
    \brief EUI64.test unit tests */

#include "EUI64.hh"

// Custom includes
#include <senf/Socket/Protocols/AddressExceptions.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(eui64)
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

SENF_AUTO_TEST_CASE(eui64_fillRight)
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
