// $Id: ICMPv6Packet.test.cc 1550 2010-01-26 09:34:24Z tho $
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
    \brief ICMPv6Packet unit tests */

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include "NDPOptions.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

///////////////////////////////cc.p////////////////////////////////////////
SENF_AUTO_UNIT_TEST(NDPMessage_create)
{
    senf::ICMPv6Packet icmp (senf::ICMPv6Packet::create());
    icmp->code() = 0;

    senf::NDPNeighborAdvertisementMessage nadm (senf::NDPNeighborAdvertisementMessage::createAfter(icmp));
    nadm->r() = true;
    nadm->s() = true;
    nadm->o() = false;
    nadm->target() = senf::INet6Address::Loopback;

    senf::NDPNeighborAdvertisementMessage::Parser::options_t::container optC(nadm->options());
    senf::NDPMTUTLVParser opt(
            optC.push_back_space().init<senf::NDPMTUTLVParser> ());
    senf::NDPTargetLLAddressTLVParser opt2(
            optC.push_back_space().init<senf::NDPTargetLLAddressTLVParser> ());
    opt.mtu() = 1234u;
    opt2.target() = senf::MACAddress::Broadcast;

    icmp.finalizeAll();

    BOOST_CHECK_EQUAL(icmp->type(), 0x88 );
    BOOST_CHECK( icmp.next() );
    BOOST_CHECK( icmp.next().is<senf::NDPNeighborAdvertisementMessage>() );
    senf::NDPNeighborAdvertisementMessage rnadm (icmp.next().as<senf::NDPNeighborAdvertisementMessage>());

    BOOST_CHECK_EQUAL( rnadm->r(), true  );
    BOOST_CHECK_EQUAL( rnadm->s(), true  );
    BOOST_CHECK_EQUAL( rnadm->o(), false );

    senf::NDPNeighborAdvertisementMessage::Parser::options_t::container roptC(rnadm->options() );
    senf::NDPNeighborAdvertisementMessage::Parser::options_t::container::iterator listIter (roptC.begin());
    BOOST_CHECK( listIter->is<senf::NDPMTUTLVParser>() );
    BOOST_CHECK_EQUAL( listIter->type(), 5u );
    BOOST_CHECK_EQUAL( listIter->length(), 1u );
    senf::NDPMTUTLVParser mtuopt (listIter->as<senf::NDPMTUTLVParser>());
    BOOST_CHECK_EQUAL( mtuopt.mtu(), 1234u );
    listIter++;
    BOOST_CHECK( listIter->is<senf::NDPTargetLLAddressTLVParser>() );
    BOOST_CHECK_EQUAL( listIter->type(), 2u );
    BOOST_CHECK_EQUAL( listIter->length(), 1u );
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
