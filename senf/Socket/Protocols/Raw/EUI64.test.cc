// $Id$
//
// Copyright (C) 2009 
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
    \brief EUI64.test unit tests */

//#include "EUI64.test.hh"
//#include "EUI64.test.ih"

// Custom includes
#include "EUI64.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(eui64)
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
