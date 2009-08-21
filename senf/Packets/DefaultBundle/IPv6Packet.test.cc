// $Id$
//
// Copyright (C) 2007
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

/** \file
    \brief IPv6Packet unit tests */

//#include "IPv6Packet.test.hh"
//#include "IPv6Packet.test.ih"

// Custom includes
#include "IPv6Packet.hh"
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include <senf/Packets/DataPacket.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ipV6Packet_parse)
{
    unsigned char data[] = { 
            0x60, 0x12, 0x20, 0x30,
            0x00, 0x01, 0x03, 0x04,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0xff 
    };

    senf::IPv6Packet p (senf::IPv6Packet::create(data));

    BOOST_CHECK_EQUAL( p->version(),       0x06u      );
    BOOST_CHECK_EQUAL( p->trafficClass(),  0x01u      );
    BOOST_CHECK_EQUAL( p->flowLabel(),     0x22030u   );
    BOOST_CHECK_EQUAL( p->length(),        0x0001u    );
    BOOST_CHECK_EQUAL( p->nextHeader(),    0x03u      );
    BOOST_CHECK_EQUAL( p->hopLimit(),      0x04u      );
    BOOST_CHECK_EQUAL( p->source().value(),
                       senf::INet6Address::from_string("1011:1213:1415:1617:1819:1a1b:1c1d:1e1f") );
    BOOST_CHECK_EQUAL( p->destination().value(),
                       senf::INet6Address::from_string("2021:2223:2425:2627:2829:2a2b:2c2d:2e2f") );

    BOOST_CHECK( p.next() );
    BOOST_CHECK( p.next().is<senf::DataPacket>() );
    BOOST_CHECK_EQUAL( p.next().size(), 1u );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

BOOST_AUTO_UNIT_TEST(ipV6Packet_create)
{
    senf::IPv6Packet ip (senf::IPv6Packet::create());

    ip->trafficClass() = 0x01u;
    ip->flowLabel() = 0x22030u;
    ip->hopLimit() = 0x04u;
    ip->source() = senf::INet6Address::from_string("1011:1213:1415:1617:1819:1a1b:1c1d:1e1f");
    ip->destination() = senf::INet6Address::from_string("2021:2223:2425:2627:2829:2a2b:2c2d:2e2f");
    
    ip.finalizeAll();
    
    unsigned char data[] = { 
            0x60, 0x12, 0x20, 0x30,
            0x00, 0x00, 0x3b, 0x04,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
    };
    BOOST_CHECK_EQUAL_COLLECTIONS( 
            data, data+sizeof(data), ip.data().begin(), ip.data().end() );
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
