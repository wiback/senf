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
    \brief INet6Address unit tests */

#include "INet6Address.hh"

// Custom includes
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include <senf/Socket/Protocols/Raw/EUI64.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/IgnoreValue.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(inet6Address)
{
    using senf::INet6Address;
    using senf::INet4Address;
    using senf::AddressSyntaxException;
    using senf::UnknownHostnameException;

    {
        INet6Address addr1 (INet6Address::from_string("0102:0304:0506:0708:090A:0B0C:0D0E:0F00"));
        BOOST_CHECK_EQUAL( addr1[0], 1 );
        BOOST_CHECK_EQUAL( addr1[1], 2 );
        BOOST_CHECK_EQUAL( addr1[2], 3 );
        BOOST_CHECK_EQUAL( addr1[3], 4 );
        BOOST_CHECK_EQUAL( addr1[4], 5 );
        BOOST_CHECK_EQUAL( addr1[5], 6 );
        BOOST_CHECK_EQUAL( addr1[6], 7 );
        BOOST_CHECK_EQUAL( addr1[7], 8 );
        BOOST_CHECK_EQUAL( addr1[8], 9 );
        BOOST_CHECK_EQUAL( addr1[9], 10 );
        BOOST_CHECK_EQUAL( addr1[10], 11 );
        BOOST_CHECK_EQUAL( addr1[11], 12 );
        BOOST_CHECK_EQUAL( addr1[12], 13 );
        BOOST_CHECK_EQUAL( addr1[13], 14 );
        BOOST_CHECK_EQUAL( addr1[14], 15 );
        BOOST_CHECK_EQUAL( addr1[15], 0 );
        SENF_CHECK_NOT_EQUAL( INet6Address::from_string("www.go6.net"), INet6Address::None );
        INet6Address addr2;
        BOOST_CHECK_EQUAL( senf::str(addr2), "::" );
        addr2 = INet6Address::from_string("::1");
        SENF_CHECK_NOT_EQUAL( addr1, addr2 );
        addr1 = INet6Address::from_string("::1");
        BOOST_CHECK_EQUAL( addr1, addr2 );
        addr1 = INet6Address::None;
        addr2 = INet6Address::from_string("::");
        BOOST_CHECK_EQUAL( addr1, addr2 );
        BOOST_CHECK_THROW( INet6Address::from_string(""), AddressSyntaxException );
        BOOST_CHECK_EQUAL( senf::str(addr1), "::" );
        unsigned char data[] = { 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x21, 0 };
        INet6Address addr3 (INet6Address::from_data(data));
        BOOST_CHECK_EQUAL( addr3, INet6Address::from_string("1200::21") );
        BOOST_CHECK_EQUAL( INet6Address::from_inet4address(INet4Address(0x01020304)),
                           INet6Address::from_string("::ffff:1.2.3.4") );
        BOOST_CHECK( INet6Address::from_inet4address(INet4Address(0x01020304)).inet4Mapped());

        BOOST_CHECK_THROW( INet6Address::from_string("1.2.3.4"), UnknownHostnameException );
        BOOST_CHECK_EQUAL( INet6Address::from_string("1.2.3.4", INet6Address::ResolveINet4),
                           INet6Address::from_string("::ffff:1.2.3.4") );
    }
    {
        INet6Address addr (INet6Address::from_string("2001:dead:beef::1002:3004"));
        BOOST_CHECK_EQUAL( addr.network(), senf::INet6Network(
                               senf::INet6Address(0x2001u,0xdeadu, 0xbeef),64) );
        BOOST_CHECK_EQUAL( addr.id(), senf::EUI64(0x0000000010023004llu) );
        BOOST_CHECK( addr.hasEUI64() );
        BOOST_CHECK( ! INet6Address::from_inet4address(INet4Address(0x01020304)).hasEUI64() );
        BOOST_CHECK( ! addr.universalId() );
        BOOST_CHECK( ! addr.groupId() );
        BOOST_CHECK( addr.unicast() );
        BOOST_CHECK( ! addr.multicast() );
        BOOST_CHECK( ! INet6Address::AllNodes.unicast() );
        BOOST_CHECK( INet6Address::AllNodes.multicast() );
        BOOST_CHECK_EQUAL( addr.scope(), INet6Address::GlobalScope );
        BOOST_CHECK( addr.globalScope() );
        BOOST_CHECK_EQUAL( INet6Address(0xfe80).scope(), INet6Address::LinkScope );
        BOOST_CHECK_EQUAL( INet6Address(0xfec0).scope(), INet6Address::SiteScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff00).scope(), INet6Address::ReservedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff01).scope(), INet6Address::InterfaceScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff02).scope(), INet6Address::LinkScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff03).scope(), INet6Address::ReservedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff04).scope(), INet6Address::AdminScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff05).scope(), INet6Address::SiteScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff06).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff07).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff08).scope(), INet6Address::OrganizationScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff09).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0a).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0b).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0c).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0d).scope(), INet6Address::UnassignedScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0e).scope(), INet6Address::GlobalScope );
        BOOST_CHECK_EQUAL( INet6Address(0xff0f).scope(), INet6Address::ReservedScope );
        BOOST_CHECK_EQUAL( INet6Address(0u,0u,0u,0u,0u,0xFFFF,0x0102u,0x0304).inet4address(),
                           INet4Address(0x01020304) );
        BOOST_CHECK( INet6Address(0u,0u,0u,0u,0u,0u,0x0102u,0x0304).inet4Compatible() );
        BOOST_CHECK( INet6Address(0u,0u,0u,0u,0u,0xFFFF,0x0102,0x0304).inet4Mapped() );
        BOOST_CHECK( ! addr.inet4Compatible() );
        BOOST_CHECK( ! addr.inet4Mapped() );

        BOOST_CHECK( INet6Address::AllNodes.globalMulticastAddr() );
        BOOST_CHECK( ! INet6Address::AllNodes.prefixMulticastAddr() );
        BOOST_CHECK( ! INet6Address::AllNodes.embeddedRpAddr() );

        BOOST_CHECK( INet6Address::Loopback );
        BOOST_CHECK( ! INet6Address::None );
    }
    {
        std::stringstream str;
        INet6Address addr;
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str ("invalid.host.fhg.de");
        INet6Address addr;
        str >> addr;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str;
        INet6Address addr (INet6Address::from_string("2001:dead:beef::1002:3004"));
        str << addr;
        BOOST_CHECK_EQUAL( str.str(), "2001:dead:beef::1002:3004");
        str >> addr;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(addr, INet6Address::from_string("2001:dead:beef::1002:3004"));
    }
    {
        INet6Address addr;
        addr.network(0x2000010203040506ull);
        BOOST_CHECK_EQUAL( addr, INet6Address(0x2000u,0x0102u,0x0304u,0x0506u) );
        addr.id(1u);
        BOOST_CHECK_EQUAL( addr, INet6Address(0x2000u,0x0102u,0x0304u,0x0506u,0u,0u,0u,1u) );
    }
    {
        using senf::MACAddress;
        BOOST_CHECK_EQUAL(
                INet6Address::from_mac( MACAddress::from_string("00-0C-29-C2-52-FF")),
                INet6Address::from_string("fe80::20c:29ff:fec2:52ff") );
        BOOST_CHECK_EQUAL(
                INet6Address::from_eui64( senf::EUI64::from_mac( MACAddress::from_string("a4:ba:db:fd:b8:76"))),
                INet6Address::from_string("fe80::a6ba:dbff:fefd:b876") );
    }
}

SENF_AUTO_TEST_CASE(inet6Network)
{
    using senf::INet6Address;
    using senf::INet6Network;
    using senf::AddressSyntaxException;

    INet6Network net (INet6Address(0xFF14u,0x1234u),32u);
    BOOST_CHECK_EQUAL( senf::str(net.address()), "ff14:1234::");
    BOOST_CHECK_EQUAL( net.prefix_len(), 32u );
    BOOST_CHECK( net );
    BOOST_CHECK( ! INet6Network() );

    INet6Network net2 ("2001:db8:1234::/44");
    BOOST_CHECK_EQUAL( net2.address(), INet6Address::from_string("2001:db8:1230::") );
    BOOST_CHECK_EQUAL( net2.prefix_len(), 44u );

    SENF_CHECK_NOT_EQUAL( net, net2 );
    BOOST_CHECK( net.match(INet6Address::from_string("ff14:1234::1")) );
    BOOST_CHECK( ! net2.match(INet6Address::from_string("ff13:1234::1")) );
    BOOST_CHECK( ! net.match(net2) );
    BOOST_CHECK( net2.match(INet6Network("2001:db8:1234::/48")) );
    BOOST_CHECK( ! net2.match(INet6Network("2001:db8:1234::/32")) );

    BOOST_CHECK( ! INet6Network("ff14:1234::1/128").match(INet6Network("ff14:1234::2/128")) );

    BOOST_CHECK_EQUAL( senf::str(net2), "2001:db8:1230::/44" );

    BOOST_CHECK_EQUAL( net2.host(0x1234u), INet6Address::from_string("2001:db8:1230::1234") );
    BOOST_CHECK_EQUAL( senf::str(net2.subnet(2u,48u)), "2001:db8:1232::/48" );

    BOOST_CHECK_THROW( senf::IGNORE(INet6Network("")), AddressSyntaxException );
    BOOST_CHECK_THROW( senf::IGNORE(INet6Network("2001:db8:1234::/beef")), AddressSyntaxException );
    BOOST_CHECK_THROW( senf::IGNORE(INet6Network("2001:db8:1234::/129")), AddressSyntaxException );

    {
        std::stringstream str;
        INet6Network net;
        str >> net;
        BOOST_CHECK( str.fail());
    }
    {
        std::stringstream str;
        INet6Network net ("2001:db8:1230::/44");
        str << net;
        BOOST_CHECK_EQUAL( str.str(), "2001:db8:1230::/44");
        str >> net;
        BOOST_CHECK( ! str.fail());
        BOOST_CHECK_EQUAL(net, INet6Network("2001:db8:1230::/44"));
    }
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
