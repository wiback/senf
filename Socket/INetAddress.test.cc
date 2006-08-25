// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Unit tests

//#include "INetAddress.test.hh"
//#include "INetAddress.test.ih"

// Custom includes
#include "INetAddress.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet4Address)
{
    using satcom::lib::INet4Address;
    using satcom::lib::InvalidINetAddressException;

    {
        INet4Address addr;
    
        addr = "127.0.0.1:12345";
        addr = std::make_pair("127.0.0.1",12345);
    }
    
    {
        INet4Address addr1("127.0.0.1:12345");
        INet4Address addr2("127.0.0.1",12345);
    }

    BOOST_CHECK( INet4Address("127.0.0.1:12345") == INet4Address("127.0.0.1",12345) );

    BOOST_CHECK_THROW( INet4Address("127.0.0.1"), InvalidINetAddressException );
    BOOST_CHECK_THROW( INet4Address("foo@bar:12345"), InvalidINetAddressException );
    BOOST_CHECK_THROW( INet4Address("127.0.0.1:1234a"), InvalidINetAddressException );
    BOOST_CHECK_THROW( INet4Address("foo@bar",12345), InvalidINetAddressException );

    BOOST_CHECK_EQUAL( INet4Address("127.0.0.1:12345").host(), "127.0.0.1" );
    BOOST_CHECK_EQUAL( INet4Address("127.0.0.1:12345").port(), 12345u );
    BOOST_CHECK_EQUAL( INet4Address("127.0.0.1:12345").str(), "127.0.0.1:12345" );

    {
        INet4Address addr("127.0.0.1:12345");
        BOOST_CHECK_EQUAL( reinterpret_cast< ::sockaddr_in * >(addr.sockaddr_p())->sin_port,
                           htons(12345) );
        BOOST_CHECK_EQUAL( reinterpret_cast< ::sockaddr_in * >(addr.sockaddr_p())->sin_addr.s_addr,
                           htonl(INADDR_LOOPBACK) );

        std::stringstream s;
        s << addr;
        BOOST_CHECK_EQUAL( s.str(), "127.0.0.1:12345" );
    }
}

BOOST_AUTO_UNIT_TEST(inet6Address)
{
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
