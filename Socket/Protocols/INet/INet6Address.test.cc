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
    \brief INet6Address.test unit tests */

//#include "INet6Address.test.hh"
//#include "INet6Address.test.ih"

// Custom includes
#include <boost/lexical_cast.hpp>
#include "INet6Address.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(inet6Address)
{
    using senf::INet6Address;
    using senf::INet4Address;

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
        BOOST_CHECK_EQUAL( INet6Address::from_string("ip6-localhost"), INet6Address::Loopback );
        INet6Address addr2;
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr2), "::" );
        addr2 = INet6Address::from_string("::1");
        BOOST_CHECK( addr1 != addr2 );
        addr1 =INet6Address::from_string("::1");
        BOOST_CHECK_EQUAL( addr1, addr2 );
        addr1 = INet6Address::None;
        addr2 = INet6Address::from_string("::");
        BOOST_CHECK_EQUAL( addr1, addr2 );
        BOOST_CHECK_THROW( INet6Address::from_string(""), INet6Address::SyntaxException );
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(addr1), "::" );
        unsigned char data[] = { 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x21, 0 };
        INet6Address addr3 (INet6Address::from_data(data));
        BOOST_CHECK_EQUAL( addr3, INet6Address::from_string("1200::21") );
        BOOST_CHECK_EQUAL( INet6Address::from_inet4address(INet4Address(0x01020304)),
                           INet6Address::from_string("::ffff:1.2.3.4") );
    }

    {
        INet6Address addr (INet6Address::from_string("2001:dead:beef::1002:3004"));
        BOOST_CHECK_EQUAL( addr.network(), 0x2001deadbeef0000llu );
        BOOST_CHECK_EQUAL( addr.id(), 0x0000000010023004llu );
        BOOST_CHECK( addr.hasEuid64() );
        BOOST_CHECK( ! INet6Address::from_inet4address(INet4Address(0x01020304)).hasEuid64() );
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
        BOOST_CHECK( INet6Address(0u,0u,0u,0u,0u,0u,0x0102u,0x0304).ipv4Compatible() );
        BOOST_CHECK( INet6Address(0u,0u,0u,0u,0u,0xFFFF,0x0102,0x0304).ipv4Mapped() );
        BOOST_CHECK( ! addr.ipv4Compatible() );
        BOOST_CHECK( ! addr.ipv4Mapped() );
    
        BOOST_CHECK( INet6Address::AllNodes.globalMulticastAddr() );
        BOOST_CHECK( ! INet6Address::AllNodes.prefixMulticastAddr() );
        BOOST_CHECK( ! INet6Address::AllNodes.embeddedRpAddr() );

        BOOST_CHECK( INet6Address::Loopback );
        BOOST_CHECK( ! INet6Address::None );
    }

    {
        INet6Address addr;
        addr.network(0x2000010203040506ull);
        BOOST_CHECK_EQUAL( addr, INet6Address(0x2000u,0x0102u,0x0304u,0x0506u) );
        addr.id(1u);
        BOOST_CHECK_EQUAL( addr, INet6Address(0x2000u,0x0102u,0x0304u,0x0506u,0u,0u,0u,1u) );
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
