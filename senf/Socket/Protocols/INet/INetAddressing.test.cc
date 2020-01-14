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
    \brief INetAddressing unit tests */

#include "INetAddressing.hh"

// Custom includes
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/IgnoreValue.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(inet4SocketAddress)
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

    BOOST_CHECK_THROW( senf::IGNORE(INet4SocketAddress("127.0.0.1")), AddressSyntaxException );
    BOOST_CHECK_THROW( senf::IGNORE(INet4SocketAddress("foo:bar")), AddressSyntaxException );
    BOOST_CHECK_THROW( senf::IGNORE(INet4SocketAddress(":12345")), AddressSyntaxException );
    BOOST_CHECK_THROW( senf::IGNORE(INet4SocketAddress("127.0.0.1:1234a")), AddressSyntaxException );

    BOOST_CHECK_EQUAL( INet4SocketAddress(12345).port(), 12345u );
    BOOST_CHECK_EQUAL( INet4SocketAddress(12345).address(), INet4Address::None );
    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345").address(), INet4Address::Loopback );
    BOOST_CHECK_EQUAL( INet4SocketAddress("127.0.0.1:12345").port(), 12345u );
    BOOST_CHECK_EQUAL( senf::str(INet4SocketAddress("127.0.0.1:12345")), "127.0.0.1:12345" );

    INet4SocketAddress addr("127.0.0.1:12345");
    BOOST_CHECK_EQUAL( reinterpret_cast< ::sockaddr_in * >(addr.sockaddr_p())->sin_port,
            htons(12345) );
    BOOST_CHECK_EQUAL( reinterpret_cast< ::sockaddr_in * >(addr.sockaddr_p())->sin_addr.s_addr,
            htonl(INADDR_LOOPBACK) );
    BOOST_CHECK_EQUAL( addr, INet4SocketAddress(addr) );

    {
        std::stringstream str;
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str ("foo:bar");
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str;
        str << addr;
        BOOST_CHECK_EQUAL( str.str(), "127.0.0.1:12345" );
        str >> addr;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(addr, INet4SocketAddress("127.0.0.1:12345"));
    }
}

SENF_AUTO_TEST_CASE(inet6SocketAddress)
{
    using senf::INet6Address;
    using senf::INet6SocketAddress;
    using senf::AddressSyntaxException;

    {
        INet6SocketAddress addr;
        BOOST_CHECK( ! addr );
        BOOST_CHECK_EQUAL( senf::str(addr.address()), "::" );
        BOOST_CHECK_EQUAL( addr.port(), 0u );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        addr = INet6SocketAddress("[12::21]:12345");
        BOOST_CHECK( addr == INet6SocketAddress("[12::21]:12345") );
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress(addr) );
        SENF_CHECK_NOT_EQUAL( addr, INet6SocketAddress("[12::21%lo]:12345") );
        BOOST_CHECK( addr );
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("12::21") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[12::21]:12345") );
        SENF_CHECK_NO_THROW( senf::IGNORE(INet6SocketAddress("www.go6.net:80")) );
        addr = INet6SocketAddress("1.2.3.4:12345", INet6Address::ResolveINet4);
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("::ffff:1.2.3.4") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );

        {
            std::stringstream str;
            str >> addr;
            BOOST_CHECK( str.fail());
        }
        {
            std::stringstream str ("[::1]");
            str >> addr;
            BOOST_CHECK( str.fail());
        }
        {
            std::stringstream str;
            str << addr;
            BOOST_CHECK_EQUAL( str.str(), "[::ffff:1.2.3.4]:12345");
            str >> addr;
            BOOST_CHECK( ! str.fail());
            BOOST_CHECK_EQUAL(addr, INet6SocketAddress("[::ffff:1.2.3.4]:12345"));
        }
    }

    {
        INet6SocketAddress addr (INet6Address::from_string("::1"), 1);
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[::1]:1") );
        BOOST_CHECK_EQUAL( addr.iface(), "" );
    }

    {
        BOOST_CHECK_THROW( INet6SocketAddress(INet6Address::Loopback, 1, "invalid_iface"),
                AddressSyntaxException );
        INet6SocketAddress addr (INet6Address::Loopback, 1, "lo");
        BOOST_CHECK_EQUAL( addr, INet6SocketAddress("[::1%lo]:1") );
        BOOST_CHECK_EQUAL( addr.iface(), "lo" );
        addr.iface("");
        BOOST_CHECK_EQUAL( addr.iface(), "" );
        addr.port(100u);
        BOOST_CHECK_EQUAL( addr.port(), 100u );
        addr.address(INet6Address::from_string("::2"));
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("::2") );
        BOOST_CHECK_THROW( senf::IGNORE(INet6SocketAddress("")), AddressSyntaxException );
        BOOST_CHECK_THROW( senf::IGNORE(INet6SocketAddress("[::1]")), AddressSyntaxException );
        BOOST_CHECK_THROW( senf::IGNORE(INet6SocketAddress("[::1]1234")), AddressSyntaxException );
        addr = INet6SocketAddress("[12::21%lo]:12345");
        BOOST_CHECK_EQUAL( senf::str(addr), "[12::21%lo]:12345" );
        BOOST_CHECK_EQUAL( addr.address(), INet6Address::from_string("12::21") );
        BOOST_CHECK_EQUAL( addr.port(), 12345u );
        BOOST_CHECK_EQUAL( addr.iface(), "lo" );
        BOOST_CHECK_EQUAL( senf::str(addr), "[12::21%lo]:12345" );
        BOOST_CHECK_EQUAL( INet6SocketAddress(12345).port(), 12345 );
        BOOST_CHECK_EQUAL( INet6SocketAddress(12345).address(), INet6Address::None );
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
