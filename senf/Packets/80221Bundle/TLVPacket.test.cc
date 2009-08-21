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
    \brief TLVPacket unit tests */

//#include "TLVPacket.test.hh"
//#include "TLVPacket.test.ih"

// Custom includes
#include "TLVPacket.hh"
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

namespace {

#define CHECK_TLVPacket(tlvPacket, ptype, plength)                            \
{                                                                             \
    BOOST_CHECK_EQUAL( tlvPacket->type(),         ptype   );                  \
    BOOST_CHECK_EQUAL( tlvPacket->length(),       plength );                  \
    BOOST_CHECK_EQUAL( tlvPacket->value().size(), int(plength) );             \
    std::ostringstream oss (std::ostringstream::out);                         \
    SENF_CHECK_NO_THROW( tlvPacket.dump( oss));                               \
    senf::PacketData::iterator dataIterator (tlvPacket->value().begin());     \
    for (unsigned i=0; i<plength; i++) {                                      \
        BOOST_CHECK_EQUAL( *dataIterator, i );                                \
        dataIterator++;                                                       \
    }                                                                         \
}
}


BOOST_AUTO_UNIT_TEST(GenericTLVPacket_parse_packet_with_simple_length)
{
    unsigned char data[] = {
        0x01, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value
    };
    GenericTLVPacket tlvPacket (GenericTLVPacket::create(data));
    CHECK_TLVPacket( tlvPacket, 0x01, 0x0Au );
}


BOOST_AUTO_UNIT_TEST(GenericTLVPacket_parse_packet_with_extended_length)
{
    unsigned char data[] = {
        0x01, // type
        0x81, // first and last bit set => one byte length following
        0x0A, // length (10 = 138 bytes value follows)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 
        0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
        0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
        0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 
        0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
        0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 
        0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89
    };
    GenericTLVPacket tlvPacket (GenericTLVPacket::create(data));
    CHECK_TLVPacket( tlvPacket, 0x01, 0x8au );
}


BOOST_AUTO_UNIT_TEST(GenericTLVPacket_create_packet_with_simple_length)
{
    unsigned char value[] = {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
    };
    GenericTLVPacket tlvPacket (GenericTLVPacket::create());
    tlvPacket->type() = 42u;
    tlvPacket->value( value);
    tlvPacket.finalizeThis();

    CHECK_TLVPacket( tlvPacket, 42u, 0x0Au );

    unsigned char data[] = {
        0x2a, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value
    };
    BOOST_CHECK( equal( tlvPacket.data().begin(), tlvPacket.data().end(), data ));
}


BOOST_AUTO_UNIT_TEST(GenericTLVPacket_create_packet_with_extended_length)
{
    unsigned char value[255];
    for (unsigned i=0; i<sizeof(value); i++)
        value[i] = i;
    GenericTLVPacket tlvPacket (GenericTLVPacket::create());
    tlvPacket->maxLengthValue( DynamicTLVLengthParser::max_value);
    SENF_CHECK_NO_THROW( tlvPacket->type() = 42u);
    SENF_CHECK_NO_THROW( tlvPacket->value( value));
    SENF_CHECK_NO_THROW( tlvPacket.finalizeThis());

    CHECK_TLVPacket( tlvPacket, 42u, sizeof(value) );

    unsigned char data[] = {
        0x2a, // type
        0x81, // first and last bit set => one byte length following
        0x7f, // length (127 = 255 bytes value)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // first bytes of value
    };
    BOOST_CHECK( equal(
            tlvPacket.data().begin(),
            boost::next( tlvPacket.data().begin(), sizeof(data)),
            data ));
}


BOOST_AUTO_UNIT_TEST(GenericTLVPacket_create_invalid_packet)
{
    GenericTLVPacket tlvPacket (GenericTLVPacket::create());
    tlvPacket->type() = 42u;
    tlvPacket.finalizeThis();

    unsigned char value[255];
    for (unsigned i=0; i<sizeof(value); i++)
        value[i] = i;

    BOOST_CHECK_THROW( tlvPacket->value( value), TLVLengthException);
    tlvPacket->maxLengthValue( sizeof(value));
    tlvPacket->value( value);
    tlvPacket.finalizeThis();
    CHECK_TLVPacket( tlvPacket, 42u, sizeof(value) );
}


namespace {

    struct TestMacAddressTLVPacketParser : public BaseTLVPacketParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( BaseTLVPacketParser );
        SENF_PARSER_VECTOR  ( value, bytes(length), senf::MACAddressParser );
        SENF_PARSER_FINALIZE( TestMacAddressTLVPacketParser );
    };

    struct TestMacAddressTLVPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TestMacAddressTLVPacketType>
    {
        typedef PacketTypeMixin<TestMacAddressTLVPacketType> mixin;
        typedef TestMacAddressTLVPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void finalize(ConcretePacket<TestMacAddressTLVPacketType> p) {
            p->finalizeLength();
        }
    };
    typedef ConcretePacket<TestMacAddressTLVPacketType> TestMacAddressTLVPacket;
}

BOOST_AUTO_UNIT_TEST(TestMacAddressTLVPacket_create)
{
    TestMacAddressTLVPacket tlvPacket (TestMacAddressTLVPacket::create());
    tlvPacket->type() = 42;
    tlvPacket->value().push_back( senf::MACAddress::from_string("01:23:45:67:89:ab") );
    tlvPacket->value().push_back( senf::MACAddress::from_string("cd:ef:01:23:45:67") );
    tlvPacket.finalizeThis();

    unsigned char data[] = {
        0x2a, // type
        0x0c, // length
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67  // value
    };
    BOOST_CHECK( equal( tlvPacket.data().begin(), tlvPacket.data().end(), data ));
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
