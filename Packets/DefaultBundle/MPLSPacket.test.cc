// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief MPLSPacket unit tests */

//#include "MPLSPacket.test.hh"
//#include "MPLSPacket.test.ih"

// Custom includes
#include "MPLSPacket.hh"
#include "EthernetPacket.hh"


#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(mplsPacket_parse)
{
    senf::PacketData::byte data[] = {
        0x00, 0x3e, 0x90, 0x80,  //1st mpls header
        0x00, 0x3e, 0xb1, 0x80,  // last mpls header
        0x00    //data
    };
    senf::MPLSPacket p (senf::MPLSPacket::create(data));

    BOOST_CHECK_EQUAL( p->label(), 1001u);
    BOOST_CHECK_EQUAL( p->tc(), 0u );
    BOOST_CHECK( !p->s());
    BOOST_CHECK_EQUAL( p->ttl(), 128u );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

BOOST_AUTO_UNIT_TEST(mplsPacket_parse_chain)
{
    senf::PacketData::byte data[] = {
        0x00, 0x3e, 0x90, 0x80,  //1st mpls header
        0x00, 0x3e, 0xb1, 0x80,  // last mpls header
        0x00    //data
    };

    senf::MPLSPacket p (senf::MPLSPacket::create(data));

    BOOST_REQUIRE( p.next().is<senf::MPLSPacket>() );
    senf::MPLSPacket p2 (p.next().as<senf::MPLSPacket>());
    BOOST_CHECK( p2->s());
    BOOST_REQUIRE( p2.next().is<senf::DataPacket>() );
}

BOOST_AUTO_UNIT_TEST(mplsPacket_create)
{
    senf::EthernetPacket eth (senf::EthernetPacket::create());
    eth->source() = senf::MACAddress::from_string("01:02:03:04:05:06");
    eth->destination() = senf::MACAddress::from_string("07:08:09:0a:0b:0c");

    senf::MPLSPacket p (senf::MPLSPacket::createAfter(eth));
    p->label()=4444u;
    p->ttl()=10u;
    p->tc()=0x0u;

    senf::MPLSPacket p2 (senf::MPLSPacket::createAfter(p));
    p->label()=5555u;
    p->ttl()=10u;
    p->tc()=0x0u;

    SENF_CHECK_NO_THROW(eth.finalizeAll());

    BOOST_REQUIRE( eth.next().is<senf::MPLSPacket>() );
    BOOST_REQUIRE( p.next().is<senf::MPLSPacket>() );
    BOOST_CHECK( p->s());
    BOOST_CHECK( !p2->s());

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
