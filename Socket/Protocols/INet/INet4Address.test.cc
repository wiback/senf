// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
#include <sstream>
#include "INet4Address.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet4Address)
{
    senf::INet4Address addr (senf::INet4Address::from_string("127.0.0.1"));
    BOOST_CHECK_EQUAL( addr, senf::INet4Address::Loopback );

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
