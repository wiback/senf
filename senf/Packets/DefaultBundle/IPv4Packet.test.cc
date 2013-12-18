// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief IPv4Packet unit tests */

//#include "IPv4Packet.test.hh"
//#include "IPv4Packet.test.ih"

// Custom includes
#include "IPv4Packet.hh"
#include "UDPPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

SENF_AUTO_UNIT_TEST(ipV4Packet_parse)
{
    unsigned char data[] = {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C, 0x11, 0x12, 0x13, 0x14,
            0x15, 0x16, 0x17, 0x18
    };

    senf::IPv4Packet p (senf::IPv4Packet::create(data));

    BOOST_CHECK_EQUAL( p->version(),     0x00u   );
    BOOST_CHECK_EQUAL( p->ihl(),         0x01u   );
    BOOST_CHECK_EQUAL( p->dscp(),        0x00u   );
    BOOST_CHECK_EQUAL( p->ecn(),         0x02u   );
    BOOST_CHECK_EQUAL( p->length(),      0x0304u );
    BOOST_CHECK_EQUAL( p->identifier(),  0x0506u );
    BOOST_CHECK_EQUAL( p->df(),          0       );
    BOOST_CHECK_EQUAL( p->mf(),          0       );
    BOOST_CHECK_EQUAL( p->frag(),        0x0708u );
    BOOST_CHECK_EQUAL( p->ttl(),         0x09u   );
    BOOST_CHECK_EQUAL( p->protocol(),    0x0Au   );
    BOOST_CHECK_EQUAL( p->checksum(),    0x0B0Cu );
    BOOST_CHECK_EQUAL( p->source().value(),      senf::INet4Address(0x11121314u) );
    BOOST_CHECK_EQUAL( p->destination().value(), senf::INet4Address(0x15161718u) );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_UNIT_TEST(ipV4Packet_create)
{
    senf::IPv4Packet ip (senf::IPv4Packet::create());

    BOOST_CHECK_EQUAL( ip->version(), 4u  );
    BOOST_CHECK_EQUAL( ip->ihl(),     5u  );
    BOOST_CHECK_EQUAL( ip.size(),     20u );

    senf::UDPPacket udp (senf::UDPPacket::createAfter(ip));

    BOOST_CHECK( ! ip->validateChecksum() );

    ip.finalizeAll();
    BOOST_CHECK_EQUAL( ip->length(),   28u    );
    BOOST_CHECK_EQUAL( ip->protocol(), 17u    );
    BOOST_CHECK_EQUAL( ip->checksum(), 0xbad2 );

    // Check, that the checksum field is correctly skipped
    ip.finalizeAll();
    BOOST_CHECK_EQUAL( ip->checksum(), 0xbad2 );

    BOOST_CHECK( ip->validateChecksum() );
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
