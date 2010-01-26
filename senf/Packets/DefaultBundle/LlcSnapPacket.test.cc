// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief LlcSnapPacket unit tests */

//#include "LlcSnapPacket.test.hh"
//#include "LlcSnapPacket.test.ih"

// Custom includes
#include "LlcSnapPacket.hh"
#include "EthernetPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(llcsnap_parse)
{
    senf::PacketData::byte data[] = {
        0xaa,             // DSAP
        0xaa,             // SSAP
        0x03,             // ctrl
        0x00, 0x00, 0x00, // Protocol Identification Field
        0x10, 0x11        // EtherType / Length
    };
    senf::LlcSnapPacket p (senf::LlcSnapPacket::create(data));

    BOOST_CHECK_EQUAL( p->dsap(), 0xaa );
    BOOST_CHECK_EQUAL( p->ssap(), 0xaa );
    BOOST_CHECK_EQUAL( p->ctrl(), 0x03 );
    BOOST_CHECK_EQUAL( p->protocolId(), 0x000000u );
    BOOST_CHECK_EQUAL( p->type_length(), 0x1011 );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_UNIT_TEST(llcSnapPacket_ethernet)
{
    senf::LlcSnapPacket llcsnap (senf::LlcSnapPacket::create());
    senf::EthernetPacket eth (senf::EthernetPacket::createAfter(llcsnap));
    senf::DataPacket payload  (senf::DataPacket::createAfter(
            eth, std::string("Hello, world!")));
    llcsnap.finalizeAll();

    BOOST_CHECK_EQUAL( llcsnap->dsap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ssap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ctrl(), 0x03 );
    BOOST_CHECK_EQUAL( llcsnap->protocolId(), 0x000000u );
    BOOST_CHECK_EQUAL( llcsnap->type_length(), 14 + 13);
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
