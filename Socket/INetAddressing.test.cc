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

//#include "INetAddressing.test.hh"
//#include "INetAddressing.test.ih"

// Custom includes
#include "INetAddressing.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet4Address)
{
    using senf::INet4Address;
    using senf::InvalidINetAddressException;

    {
        INet4Address addr;
    
        addr = "127.0.0.1:12345";
    }
    
    {
        INet4Address addr1("127.0.0.1:12345");
        INet4Address addr2(std::string("127.0.0.1:12345"));
        INet4Address addr3("127.0.0.1",12345);
    }

    BOOST_CHECK_EQUAL( INet4Address("127.0.0.1:12345"), INet4Address("127.0.0.1",12345) );

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
    using senf::INet6Address;
    using senf::INet6SocketAddress;
    using senf::InvalidINetAddressException;

    {
	INet6Address addr1 ("0102:0304:0506:0708:090A:0B0C:0D0E:0F00");
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[0], 1 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[1], 2 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[2], 3 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[3], 4 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[4], 5 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[5], 6 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[6], 7 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[7], 8 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[8], 9 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[9], 10 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[10], 11 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[11], 12 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[12], 13 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[13], 14 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[14], 15 );
	BOOST_CHECK_EQUAL( addr1.addr().s6_addr[15], 0 );
	INet6Address addr2;
	BOOST_CHECK_EQUAL( addr2, "::" );
	addr2 = "::1";
	BOOST_CHECK( addr1 != addr2 );
	addr1 ="::1";
	BOOST_CHECK_EQUAL( addr1, addr2 );
	BOOST_CHECK_EQUAL( addr1.address(),"::1" );
	addr1.clear();
	addr2 = "::";
	BOOST_CHECK_EQUAL( addr1, addr2 );
	BOOST_CHECK_THROW( addr1 = "", InvalidINetAddressException );
	BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr1), "::" );
    }

    {
	INet6SocketAddress addr;
	BOOST_CHECK_EQUAL( addr.host(), "::" );
	BOOST_CHECK_EQUAL( addr.port(), 0u );
	BOOST_CHECK_EQUAL( addr.iface(), "" );
	addr = "[12::21]:12345";
	BOOST_CHECK_EQUAL( addr.host(), "12::21" );
	BOOST_CHECK_EQUAL( addr.port(), 12345u );
	BOOST_CHECK_EQUAL( addr.iface(), "" );
	BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[12::21]:12345") );
    }
    
    {
	INet6SocketAddress addr ("::1", 1);
	BOOST_CHECK_EQUAL( addr, "[::1]:1" );
	BOOST_CHECK_EQUAL( addr.iface(), "" );
    }

    {
	INet6SocketAddress addr ("::1", 1, "lo");
	BOOST_CHECK_EQUAL( addr, "[::1@lo]:1" );
	BOOST_CHECK_EQUAL( addr.iface(), "lo" );
	addr.iface("");
	BOOST_CHECK_EQUAL( addr.iface(), "" );
	addr.port(100u);
	BOOST_CHECK_EQUAL( addr.port(), 100u );
	addr.host("::2");
	BOOST_CHECK_EQUAL( addr.host(), "::2" );
	BOOST_CHECK_THROW( addr = "", InvalidINetAddressException );
	BOOST_CHECK_THROW( addr = "[::1]", InvalidINetAddressException );
	BOOST_CHECK_THROW( addr = "[::1]1234", InvalidINetAddressException );
	addr = "[12::21@lo]:12345";
	BOOST_CHECK_EQUAL( addr.address(), "[12::21@lo]:12345" );
	BOOST_CHECK_EQUAL( addr.host(), "12::21" );
	BOOST_CHECK_EQUAL( addr.port(), 12345u );
	BOOST_CHECK_EQUAL( addr.iface(), "lo" );
	BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr), "[12::21@lo]:12345" );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
