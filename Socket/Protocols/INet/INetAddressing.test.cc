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
    using senf::INet4SocketAddress;
    using senf::INet4Address;

    {
        INet4SocketAddress addr;
        
        BOOST_CHECK( ! addr );

        addr = INet4SocketAddress("127.0.0.1:12345");
        BOOST_CHECK ( addr != INet4SocketAddress() );
    }

    {
        INet4SocketAddress addr1("127.0.0.1:12345");
        INet4SocketAddress addr3(INet4Address::Loopback,12345);
    }

    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345"), 
                       INet4SocketAddress(INet4Address::Loopback,12345) );

    BOOST_CHECK_THROW( INet4SocketAddress("127.0.0.1"), INet4SocketAddress::SyntaxException );
    BOOST_CHECK_THROW( INet4SocketAddress("foo@bar:12345"), INet4Address::SyntaxException );
    BOOST_CHECK_THROW( INet4SocketAddress("127.0.0.1:1234a"), INet4SocketAddress::SyntaxException );

    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345").address(), INet4Address::Loopback );
    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345").port(), 12345u );
    BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(INet4SocketAddress("127.0.0.1:12345")),
                       "127.0.0.1:12345" );

    {
        INet4SocketAddress addr("127.0.0.1:12345");
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
        BOOST_CHECK_THROW( addr1 = "", INet6Address::SyntaxException );
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr1), "::" );
        unsigned char data[] = { 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x21, 0 };
        INet6Address addr3 (std::make_pair(&data[0],&data[0]+sizeof(data)-1));
        BOOST_CHECK_EQUAL( addr3, "1200::21" );
        BOOST_CHECK_THROW( INet6Address(std::make_pair(&data[0],&data[0]+sizeof(data))),
                           INet6Address::SyntaxException );
        BOOST_CHECK_THROW( INet6Address(std::make_pair(&data[0],&data[0]+sizeof(data)-2)),
                           INet6Address::SyntaxException );
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
        BOOST_CHECK_THROW( addr = "", INet6SocketAddress::SyntaxException );
        BOOST_CHECK_THROW( addr = "[::1]", INet6SocketAddress::SyntaxException );
        BOOST_CHECK_THROW( addr = "[::1]1234", INet6SocketAddress::SyntaxException );
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
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
