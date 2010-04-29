// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Sauer <ssauer@berlios.de>
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
    \brief NDPMessage unit tests */

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include "NDPOptions.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

///////////////////////////////cc.p////////////////////////////////////////
SENF_AUTO_UNIT_TEST(NDPMessage_create)
{
    unsigned char data[] = {
            0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, //IPv6
            0x20, 0x01, 0x08, 0x90, 0x06, 0x00, 0xff, 0xff,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01,
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, 0x03, 0x00, 0x02,
            0x87, 0x00, 0xaf, 0x30,                         //ICMPv6
            0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x08, 0x90, //Neighbor Solicitation
            0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x03, 0x00, 0x02,
            0x01, 0x01, 0x00, 0x0d,                         //Source Option
            0x88, 0x70, 0xe4, 0xc0,
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create(data));
    senf::ICMPv6Packet icmp (ip.find<senf::ICMPv6Packet>());

    BOOST_CHECK_EQUAL(icmp->type(), 0x87 );
    BOOST_CHECK( icmp.next() );
    BOOST_CHECK( icmp.next().is<senf::NDPNeighborSolicitationMessage>() );
    senf::NDPNeighborSolicitationMessage rnadm (icmp.next().as<senf::NDPNeighborSolicitationMessage>());
    BOOST_CHECK_EQUAL(rnadm.size(),28);

    senf::NDPNeighborSolicitationMessage::Parser::options_t::container roptC(rnadm->options() );
    senf::NDPNeighborSolicitationMessage::Parser::options_t::container::const_iterator listIter (roptC.begin());
    BOOST_CHECK( listIter->is<senf::NDPSourceLLAddressTLVParser>() );
    BOOST_CHECK_EQUAL( listIter->type(), 1u );
    BOOST_CHECK_EQUAL( listIter->length(), 1u );
    senf::NDPSourceLLAddressTLVParser llopt (listIter->as<senf::NDPSourceLLAddressTLVParser>());
}

///////////////////////////////cc.e////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
