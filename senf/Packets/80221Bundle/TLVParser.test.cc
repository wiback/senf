// $Id$
//
// Copyright (C) 2007
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief TLVParser unit tests */

//#include "TLVParser.test.hh"
//#include "TLVParser.test.ih"

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include "TLVParser.hh"
#include "Exceptions.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

namespace {
    struct VoidPacket : public PacketTypeBase
    {};
}

#define CHECK_TLVParser(tlvParser, ptype, plength)                          \
{                                                                           \
    BOOST_CHECK_EQUAL( tlvParser.type(),         ptype   );                 \
    BOOST_CHECK_EQUAL( tlvParser.length(),       plength );                 \
    BOOST_CHECK_EQUAL( tlvParser.value().size(), int(plength) );            \
    senf::PacketData::iterator dataIterator (tlvParser.value().begin());    \
    for (unsigned i=0; i<plength; i++) {                                    \
        BOOST_CHECK_EQUAL( *dataIterator, i );                              \
        dataIterator++;                                                     \
    }                                                                       \
}


SENF_AUTO_UNIT_TEST(MIHGenericTLVParser_parse_with_simple_length)
{
    PacketInterpreterBase::byte data[] = {
        0x01, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value
    };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));
    MIHGenericTLVParser tlvParser( p->data().begin(), &p->data());
    CHECK_TLVParser( tlvParser, 0x01, 0x0Au );
}


SENF_AUTO_UNIT_TEST(MIHGenericTLVParser_parse_with_extended_length)
{
    PacketInterpreterBase::byte data[] = {
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
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(data));
    MIHGenericTLVParser tlvParser( p->data().begin(), &p->data());
    CHECK_TLVParser( tlvParser, 0x01, 0x8au );
}


SENF_AUTO_UNIT_TEST(MIHGenericTLVParser_create_with_simple_length)
{
    PacketInterpreterBase::byte value[] = {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
    };
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(2u)));
    MIHGenericTLVParser tlvParser( p->data().begin(), &p->data());
    tlvParser.type() = 42u;
    tlvParser.value( value);
    tlvParser.finalize();

    CHECK_TLVParser( tlvParser, 42u, 0x0Au );

    PacketInterpreterBase::byte data[] = {
        0x2a, // type
        0x0A, // first bit not set, length=10
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // value
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            p->data().begin(), p->data().end() );
}


SENF_AUTO_UNIT_TEST(MIHGenericTLVParser_create_with_extended_length)
{
    PacketInterpreterBase::byte value[255];
    for (unsigned i=0; i<sizeof(value); i++)
        value[i] = i;
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(2u)));
    MIHGenericTLVParser tlvParser( p->data().begin(), &p->data());
    tlvParser.maxLength( MIHLengthParser::max_value);
    tlvParser.type() = 42u;
    tlvParser.value( value);
    tlvParser.finalize();

    CHECK_TLVParser( tlvParser, 42u, sizeof(value) );

    PacketInterpreterBase::byte data[] = {
        0x2a, // type
        0x81, // first and last bit set => one byte length following
        0x7f, // length (127 = 255 bytes value)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // first bytes of value
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            p->data().begin(), boost::next( p->data().begin(), sizeof(data)) );
}


SENF_AUTO_UNIT_TEST(MIHGenericTLVParser_create_invalid)
{
    PacketInterpreterBase::ptr p (PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(2u)));
    MIHGenericTLVParser tlvParser( p->data().begin(), &p->data());
    tlvParser.type() = 42u;
    tlvParser.finalize();

    PacketInterpreterBase::byte value[255];
    for (unsigned i=0; i<sizeof(value); i++)
        value[i] = i;

    BOOST_CHECK_THROW( tlvParser.value( value), MIHLengthException);
    tlvParser.maxLength( sizeof(value));
    tlvParser.value( value);
    tlvParser.finalize();
    CHECK_TLVParser( tlvParser, 42u, sizeof(value) );
}


namespace {

    struct TestMacAddressTLVPacketParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( MIHBaseTLVParser );
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
            p->finalize();
        }
    };
    typedef ConcretePacket<TestMacAddressTLVPacketType> TestMacAddressTLVPacket;
}

SENF_AUTO_UNIT_TEST(TestMacAddressTLVPacket_create)
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
    SENF_CHECK_EQUAL_COLLECTIONS(
            data, data+sizeof(data), tlvPacket.data().begin(), tlvPacket.data().end() );
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
