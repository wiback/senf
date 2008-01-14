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

// Unit tests

//#include "TLVPacket.test.hh"
//#include "TLVPacket.test.ih"

// Custom includes
#include "TLVPacket.hh"
#include <senf/Packets.hh>

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <vector>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;


template <class TLVPacketType>
void check_TLVPacket(TLVPacketType tlvPacket, boost::uint32_t type, boost::uint32_t length)
{
    BOOST_CHECK_EQUAL( tlvPacket->type(), type );
    BOOST_CHECK_EQUAL( tlvPacket->length(), length );

    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK_EQUAL( tlvPacket_value.size(), length);
    for (int i=0, j=tlvPacket_value.size(); i<j; i++)
        BOOST_CHECK_EQUAL( tlvPacket_value[i], i );
}


BOOST_AUTO_UNIT_TEST(TLVPacket_static)
{
    // check static values:
    // number of bytes to allocate for a new TLVPacket should be 5
    // BOOST_CHECK_EQUAL( TLVPacket::type::initSize(), 5u );
}


BOOST_AUTO_UNIT_TEST(TLVPacket_parse_packet_with_simple_length)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, DynamicTLVLengthParser> > TestTLVPacket;
    unsigned char data[] = { 
        0x01, 0x23, 0x45, 0x67, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value (payload)
    };
    TestTLVPacket tlvPacket (TestTLVPacket::create(data));
    check_TLVPacket( tlvPacket, 0x01234567u, 0x0Au );
}


BOOST_AUTO_UNIT_TEST(TLVPacket_parse_packet_with_extended_length)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, DynamicTLVLengthParser> > TestTLVPacket;
    unsigned char data[] = { 
        0x01, 0x23, 0x45, 0x67, // type
        0x81, // first and last bit set => one byte length following
        0x0A, // length (10 bytes value)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value (payload)
    };        
    TestTLVPacket tlvPacket (TestTLVPacket::create(data));
    check_TLVPacket( tlvPacket, 0x01234567u, 0x0Au );
}


BOOST_AUTO_UNIT_TEST(TLVPacket_create_packet_with_simple_length)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, DynamicTLVLengthParser> > TestTLVPacket;
    std::string payload ("Hello, world!");
    TestTLVPacket tlvPacket (TestTLVPacket::create());
    tlvPacket->type() = 42u;
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();

    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u);
    BOOST_CHECK_EQUAL( tlvPacket->length(), 13u);
    
    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK( equal( tlvPacket_value.begin(), tlvPacket_value.end(), payload.begin() ));
}


BOOST_AUTO_UNIT_TEST(TLVPacket_create_packet_with_extended_length)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, DynamicTLVLengthParser> > TestTLVPacket;
    std::string payload (
            "This is a very long string with more than 127 characters to check if the TLV-Packet "
            "works correctly with an extended length. That's all." );
    TestTLVPacket tlvPacket (TestTLVPacket::create());
    tlvPacket->type() = 42u;
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();
        
    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u );
    BOOST_CHECK_EQUAL( tlvPacket->length(), payload.size() );
    
    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK( equal( tlvPacket_value.begin(), tlvPacket_value.end(), payload.begin() ));

    payload = std::string("This is a short string with less than 127 characters. That's all.");
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();

    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u );
    BOOST_CHECK_EQUAL( tlvPacket->length(), payload.size() );
	    
    PacketData & tlvPacket_value2 (tlvPacket.next().data());
    BOOST_CHECK( equal( tlvPacket_value2.begin(), tlvPacket_value2.end(), payload.begin() ));	   
}


BOOST_AUTO_UNIT_TEST(TLVPacket_create_invalid_packet)
{
    
}


BOOST_AUTO_UNIT_TEST(TLVFixPacket_static)
{
    // check static values:
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt8Parser> > TestTLVPacket8;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt24Parser> > TestTLVPacket24;
    
    BOOST_CHECK_EQUAL( TestTLVPacket8::type::initSize(),  4+1u );
    BOOST_CHECK_EQUAL( TestTLVPacket24::type::initSize(), 4+3u );
}


template <class TLVFixPacketType>
void test_TLVFixPacket_parsing(unsigned lengthParser_size)
{
    std::vector<char> data;
    data.push_back(0x01); data.push_back(0x23); data.push_back(0x45); data.push_back(0x67); // type
    data.insert(data.end(), lengthParser_size-1, 0x00);
    data.push_back(0x0A); // length
    for( int i=0; i < 10; i++ ) {
        data.push_back(i); // payload
    }
    TLVFixPacketType tlvPacket (TLVFixPacketType::create(
            boost::make_iterator_range(data.begin(), data.end())));
    check_TLVPacket( tlvPacket, 0x01234567u, 0x0Au );
}

BOOST_AUTO_UNIT_TEST(TLVFixPacket_parse_packet)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt8Parser> > TestTLVPacket8;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt16Parser> > TestTLVPacket16;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt24Parser> > TestTLVPacket24;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt32Parser> > TestTLVPacket32;
    
    test_TLVFixPacket_parsing<TestTLVPacket8>( UInt8Parser::fixed_bytes);
    test_TLVFixPacket_parsing<TestTLVPacket16>( UInt16Parser::fixed_bytes);
    test_TLVFixPacket_parsing<TestTLVPacket24>( UInt24Parser::fixed_bytes);
    test_TLVFixPacket_parsing<TestTLVPacket32>( UInt32Parser::fixed_bytes);
}


template <class TLVFixPacketType>
void test_TLVFixPacket_creating()
{
    std::string payload ("Hello, world!");
    TLVFixPacketType tlvPacket (TLVFixPacketType::create());
    tlvPacket->type() = 42u;
    DataPacket::createAfter( tlvPacket, payload );
    tlvPacket.finalize();

    BOOST_CHECK_EQUAL( tlvPacket->type(), 42u);
    BOOST_CHECK_EQUAL( tlvPacket->length(), 13u);
    
    PacketData & tlvPacket_value (tlvPacket.next().data());
    BOOST_CHECK( equal( tlvPacket_value.begin(), tlvPacket_value.end(), payload.begin() ));
}

BOOST_AUTO_UNIT_TEST(TLVFixPacket_create_packet)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt8Parser> > TestTLVPacket8;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt16Parser> > TestTLVPacket16;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt24Parser> > TestTLVPacket24;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt32Parser> > TestTLVPacket32;
    
    test_TLVFixPacket_creating<TestTLVPacket8>();
    test_TLVFixPacket_creating<TestTLVPacket16>();
    test_TLVFixPacket_creating<TestTLVPacket24>();
    test_TLVFixPacket_creating<TestTLVPacket32>();
}


template <class TLVFixPacketType>
void test_invalid_TLVFixPacket_creating(boost::uint32_t max_value)
{
    TLVFixPacketType tlvPacket (TLVFixPacketType::create());
    tlvPacket->type() = 42u;
    DataPacket payload (DataPacket::createAfter( tlvPacket, max_value+1));
    //DataPacket::createAfter( payload, 1); // this is one byte to much.
    BOOST_CHECK_THROW( tlvPacket.finalize(), UnsuportedTLVPacketException);
}

BOOST_AUTO_UNIT_TEST(TLVFixPacket_create_invalid_packet)
{
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt8Parser> > TestTLVPacket8;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt16Parser> > TestTLVPacket16;
    typedef ConcretePacket<TLVPacketType<UInt32Parser, UInt24Parser> > TestTLVPacket24;
    
    test_invalid_TLVFixPacket_creating<TestTLVPacket8> ( UInt8Parser::max_value);
    test_invalid_TLVFixPacket_creating<TestTLVPacket16>( UInt16Parser::max_value);
    test_invalid_TLVFixPacket_creating<TestTLVPacket24>( UInt24Parser::max_value);
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
