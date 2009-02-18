// $Id$
//
// Copyright (C) 2006
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

// Unit tests

//#include "INetAddressing.test.hh"
//#include "INetAddressing.test.ih"

// Custom includes
#include "INetAddressing.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet4SocketAddress)
{
    using senf::INet4SocketAddress;
    using senf::INet4Address;
    using senf::AddressSyntaxException;

    {
        INet4SocketAddress addr;

        BOOST_CHECK( ! addr );

        addr = INet4SocketAddress("127.0.0.1:12345");
        BOOST_CHECK ( addr != INet4SocketAddress("127.0.0.2:12345") );
    }

    {
        INet4SocketAddress addr1("127.0.0.1:12345");
        INet4SocketAddress addr3(INet4Address::Loopback,12345);
    }

    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345"),
                       INet4SocketAddress(INet4Address::Loopback,12345) );

    BOOST_CHECK_THROW( INet4SocketAddress("127.0.0.1"), AddressSyntaxException );
    BOOST_CHECK_THROW( INet4SocketAddress("foo:bar"), AddressSyntaxException );
    BOOST_CHECK_THROW( INet4SocketAddress(":12345"), AddressSyntaxException );
    BOOST_CHECK_THROW( INet4SocketAddress("127.0.0.1:1234a"), AddressSyntaxException );

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

        BOOST_CHECK_EQUAL( addr, INet4SocketAddress(addr) );

        std::stringstream str;
        str >> addr;
        BOOST_CHECK( str.fail());
        str.clear();
        str << addr;
        BOOST_CHECK_EQUAL( str.str(), "127.0.0.1:12345" );
        str >> addr;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(addr, INet4SocketAddress("127.0.0.1:12345"));
    }
}

BOOST_AUTO_UNIT_TEST(inet6SocketAddress)
{
    using senf::INet6Address;
    using senf::INet6SocketAddress;
    using senf::AddressSyntaxException;

    {
        INet6SocketAddress addr;
        BOOST_CHECK( ! addr );
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr.address()), "::" );
        BOOST_CHECK_EQUAL( addr.port(), 0u );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        addr = INet6SocketAddress("[12::21]:12345");
        BOOST_CHECK( addr == INet6SocketAddress("[12::21]:12345") );
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress(addr) );
        BOOST_CHECK( addr != INet6SocketAddress("[12::21%lo]:12345") );
        BOOST_CHECK( addr );
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("12::21") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[12::21]:12345") );
        SENF_CHECK_NO_THROW( INet6SocketAddress("www.6bone.net:80") );
        addr = INet6SocketAddress("1.2.3.4:12345", INet6Address::ResolveINet4);
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("::ffff:1.2.3.4") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );

        std::stringstream str;
        str >> addr;
        BOOST_CHECK( str.fail());
        str.clear();
        str << addr;
        BOOST_CHECK_EQUAL( str.str(), "[::ffff:1.2.3.4]:12345");
        str >> addr;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(addr, INet6SocketAddress("[::ffff:1.2.3.4]:12345"));
    }

    {
        INet6SocketAddress addr (INet6Address::from_string("::1"), 1);
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[::1]:1") );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
    }

    {
        INet6SocketAddress addr (INet6Address::Loopback, 1, "lo");
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[::1%lo]:1") );
        BOOST_CHECK_EQUAL( addr.iface(), "lo" );
        addr.iface("");
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        addr.port(100u);
        BOOST_CHECK_EQUAL( addr.port(), 100u );
        addr.address(INet6Address::from_string("::2"));
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("::2") );
        BOOST_CHECK_THROW( INet6SocketAddress(""), AddressSyntaxException );
        BOOST_CHECK_THROW( INet6SocketAddress("[::1]"), AddressSyntaxException );
        BOOST_CHECK_THROW( INet6SocketAddress("[::1]1234"), AddressSyntaxException );
        addr = INet6SocketAddress("[12::21%lo]:12345");
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr), "[12::21%lo]:12345" );
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("12::21") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );
        BOOST_CHECK_EQUAL( addr.iface(), "lo" );
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr), "[12::21%lo]:12345" );
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
