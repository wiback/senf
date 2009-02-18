// $Id$
//
// Copyright (C) 2009
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
    \brief MIHPacket unit tests */

//#include "MIHPacket.test.hh"
//#include "MIHPacket.test.ih"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#include "MIHPacket.hh"

using namespace senf;

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(MIHPacket_create_string)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().setString( "senf@berlios.de");
    mihPacket->dst_mihfId().setString( "test");
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x17,
            // source MIHF_ID TLV:
            0x01, 0x0f, // type, length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value
            // destination MIHF_ID TLV:
            0x02, 0x04, 0x74, 0x65, 0x73, 0x74
    };
    BOOST_CHECK(equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
    BOOST_CHECK_EQUAL( mihPacket->src_mihfId().asString(), "senf@berlios.de");
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().asString(), "test");

    // now expand a MIHF_ID
    mihPacket->dst_mihfId().maxLengthValue(253);
    mihPacket->dst_mihfId().setString( std::string(200, 'x'));
    mihPacket.finalizeThis();

    BOOST_CHECK_EQUAL( mihPacket.size(), unsigned(8 + 17 + 203));
    BOOST_CHECK_EQUAL( mihPacket->payloadLength(), 17 + 203);
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().length(), 200u);
    BOOST_CHECK_EQUAL( senf::bytes(mihPacket->dst_mihfId()), 203u);

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( mihPacket.dump( oss));
}


BOOST_AUTO_UNIT_TEST(MIHPacket_create_mac)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().setMACAddress( MACAddress::from_string("01:02:03:04:05:06"));
    mihPacket->dst_mihfId().setMACAddress( MACAddress::from_string("07:08:09:0a:0b:0c"));
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x1c,
            // source MIHF_ID TLV:
            0x01, 0x0c, // type, length
            0x5c, 0x01, 0x5c, 0x02, 0x5c, 0x03, 0x5c, 0x04, 0x5c, 0x05, 0x5c, 0x06, // value (nai-encoded)
            // destination MIHF_ID TLV:
            0x02, 0x0c,  // type, length
            0x5c, 0x07, 0x5c, 0x08, 0x5c, 0x09, 0x5c, 0x0a, 0x5c, 0x0b, 0x5c, 0x0c  // value (nai-encoded)
    };
    BOOST_CHECK(equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
    BOOST_CHECK_EQUAL(
            mihPacket->src_mihfId().asMACAddress(),
            MACAddress::from_string("01:02:03:04:05:06"));
    BOOST_CHECK_EQUAL(
            mihPacket->dst_mihfId().asMACAddress(),
            MACAddress::from_string("07:08:09:0a:0b:0c"));
}


BOOST_AUTO_UNIT_TEST(MIHPacket_create_inet4)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().setINet4Address( INet4Address::from_string("128.129.130.131"));
    mihPacket->dst_mihfId().setINet4Address( INet4Address::from_string("132.133.134.135"));
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x14,
            // source MIHF_ID TLV:
            0x01, 0x08, // type, length
            0x5c, 0x80, 0x5c, 0x81, 0x5c, 0x82, 0x5c, 0x83, // value (nai-encoded)
            // destination MIHF_ID TLV:
            0x02, 0x08, // type, length
            0x5c, 0x84, 0x5c, 0x85, 0x5c, 0x86, 0x5c, 0x87  // value (nai-encoded)
    };
    BOOST_CHECK(equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
    BOOST_CHECK_EQUAL(
            mihPacket->src_mihfId().asINet4Address(),
            INet4Address::from_string("128.129.130.131"));
    BOOST_CHECK_EQUAL(
            mihPacket->dst_mihfId().asINet4Address(),
            INet4Address::from_string("132.133.134.135"));
}


BOOST_AUTO_UNIT_TEST(MIHPacket_create_inet6)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().setINet6Address( INet6Address::from_string("::ffff:1.2.3.4"));
    mihPacket->dst_mihfId().setINet6Address( INet6Address::from_string("::ffff:5.6.7.8"));
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x44,
            // source MIHF_ID TLV:
            0x01, 0x20, // type, length
            // value (nai-encoded):
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0xff, 0x5c, 0xff,
            0x5c, 0x01, 0x5c, 0x02, 0x5c, 0x03, 0x5c, 0x04,
            // destination MIHF_ID TLV:
            0x02, 0x20, // type, length
            // value (nai-encoded):
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0xff, 0x5c, 0xff,
            0x5c, 0x05, 0x5c, 0x06, 0x5c, 0x07, 0x5c, 0x08
    };
    BOOST_CHECK(equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
    BOOST_CHECK_EQUAL(
            mihPacket->src_mihfId().asINet6Address(),
            INet6Address::from_string("::ffff:1.2.3.4"));
    BOOST_CHECK_EQUAL(
            mihPacket->dst_mihfId().asINet6Address(),
            INet6Address::from_string("::ffff:5.6.7.8") );
}


BOOST_AUTO_UNIT_TEST(MIHPayload_parse)
{
    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15,
            // variable payload length:
            0x00, 0x2a,
            // source MIHF_ID TLV:
            0x01, 0x0f, // type, length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value ("senf@berlios.de")
            // destination MIHF_ID TLV:
            0x02, 0x04, 0x74, 0x65, 0x73, 0x74,  // type, length, value ("test")
            // MIH Payload (two test tlvs)
            // first test tlv
            0x42, // type
            0x0a, // first bit not set, length=10
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // value
            // second test tlv
            0x43, // type
            0x05, // first bit not set, length=5
            0x1a, 0x2b, 0x3c, 0x4d, 0x5e // value
    };

    MIHPacket mihPacket (MIHPacket::create(data));
    BOOST_CHECK_EQUAL( mihPacket->payloadLength(), 42u);

    BOOST_REQUIRE( mihPacket.next().is<MIHPayloadPacket>() );
    MIHPayloadPacket mihPayload (mihPacket.next().as<MIHPayloadPacket>());

    BOOST_CHECK_EQUAL( mihPayload->tlv_list().size(), 2u);
    MIHPayloadPacketParser::tlv_list_t::container tlv_list_container (mihPayload->tlv_list());

    GenericTLVPacket::Parser tlv1 = *tlv_list_container.begin();
    BOOST_CHECK_EQUAL( tlv1.type(), 0x42);
    BOOST_CHECK_EQUAL( tlv1.length(), 0x0au);
    BOOST_CHECK_EQUAL( tlv1.value().size(), 0x0a);

    GenericTLVPacket::Parser tlv2 = *boost::next(tlv_list_container.begin());
    BOOST_CHECK_EQUAL( tlv2.type(), 0x43);
    BOOST_CHECK_EQUAL( tlv2.length(), 0x05u);
    BOOST_CHECK_EQUAL( tlv2.value().size(), 0x05);
}


BOOST_AUTO_UNIT_TEST(MIHPayload_create)
{
    MIHPacket mihPacket (MIHPacket::create());
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().setString( "senf@berlios.de");
    mihPacket->dst_mihfId().setString( "test");

    MIHPayloadPacket mihPayload (MIHPayloadPacket::createAfter(mihPacket));

    unsigned char tlv1_value[] = {
            0x1a, 0x2b, 0x3c, 0x4d, 0x5e };
    GenericTLVPacket tlv2 = (GenericTLVPacket::create());
    tlv2->type() = 0x43;
    tlv2->value( tlv1_value);
    tlv2.finalizeThis();
    mihPayload->tlv_list().push_front( tlv2.parser());

    unsigned char tlv2_value[] = {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
    GenericTLVPacket tlv1 (GenericTLVPacket::create());
    tlv1->type() = 0x42;
    tlv1->value( tlv2_value);
    tlv1.finalizeThis();
    mihPayload->tlv_list().push_front( tlv1.parser());

    mihPacket.finalizeAll();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15,
            // variable payload length:
            0x00, 0x2a,
            // source MIHF_ID TLV:
            0x01, 0x0f, // type, length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value ("senf@berlios.de")
            // destination MIHF_ID TLV:
            0x02, 0x04, 0x74, 0x65, 0x73, 0x74,  // type, length, value ("test")
            // MIH Payload (two test tlvs)
            // first test tlv
            0x42, // type
            0x0a, // first bit not set, length=10
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // value
            // second test tlv
            0x43, // type
            0x05, // first bit not set, length=5
            0x1a, 0x2b, 0x3c, 0x4d, 0x5e // value
    };

    BOOST_CHECK(equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
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
