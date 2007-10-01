// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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

//#include "TLVPacket.test.hh"
//#include "TLVPacket.test.ih"

// Custom includes
#include "TLVPacket.hh"
#include <senf/Packets.hh>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(tlvPacket_static)
{
    // check static values:
    // number of bytes to allocate for a new TLVPacket should be 5
    BOOST_CHECK_EQUAL( init_bytes<Parse_TLVPacket>::value, 5u );
    BOOST_CHECK_EQUAL( TLVPacketType::initSize(), 5u );
}

BOOST_AUTO_UNIT_TEST(tlvPacket_parse_packet_with_simple_length)
{
    unsigned char data[] = { 
        0x01, 0x23, 0x45, 0x67, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value (payload)
    };
            
    senf::TLVPacket tlvPacket (senf::TLVPacket::create(data));
    
    BOOST_CHECK_EQUAL( tlvPacket->type(), 0x01234567u );
    BOOST_CHECK_EQUAL( tlvPacket->length(), 0x0Au );

    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK_EQUAL( tlvPacket_value.size(), 0x0Au);
    for (int i=0, j=tlvPacket_value.size(); i<j; i++)
        BOOST_CHECK_EQUAL( tlvPacket_value[i], i );
}

BOOST_AUTO_UNIT_TEST(tlvPacket_parse_packet_with_extended_length)
{
    unsigned char data[] = { 
        0x01, 0x23, 0x45, 0x67, // type
        0x81, // first and last bit set => one byte length following
        0x0A, // length (10 bytes value)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value (payload)
    };
            
    senf::TLVPacket tlvPacket (senf::TLVPacket::create(data));
    
    BOOST_CHECK_EQUAL( tlvPacket->type(), 0x01234567u );
    BOOST_CHECK_EQUAL( tlvPacket->length(), 0x0Au );

    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK_EQUAL( tlvPacket_value.size(), 0x0Au);
    for (int i=0, j=tlvPacket_value.size(); i<j; i++)
        BOOST_CHECK_EQUAL( tlvPacket_value[i], i );
}

BOOST_AUTO_UNIT_TEST(tlvPacket_create_packet_with_simple_length)
{
    std::string payload ("Hello, world!");
    TLVPacket tlvPacket (TLVPacket::create());
    tlvPacket->type() = 42u;
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();

    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u);
    BOOST_CHECK_EQUAL( tlvPacket->length(), 13u);
    
    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK( equal( tlvPacket_value.begin(), tlvPacket_value.end(), payload.begin() ));
}

/*
BOOST_AUTO_UNIT_TEST(tlvPacket_create_packet_with_extended_length)
{
    std::string payload (
            "This is a very long string with more than 127 characters to check if the TLV-Packet "
            "works correctly with an extended length. That's all." );
    TLVPacket tlvPacket (TLVPacket::create( payload.size() + 4 + 2));
    tlvPacket->type() = 42u;
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();
        
    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u );
    BOOST_CHECK_EQUAL( tlvPacket->length(), payload.size() );
    
    PacketData & tlvPacket_value (tlvPacket.next().data());
   
    BOOST_CHECK( equal( tlvPacket_value.begin(), tlvPacket_value.end(), payload.begin() ));
}
*/

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
