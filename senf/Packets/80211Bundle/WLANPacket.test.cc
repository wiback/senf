// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Christian Niephaus <christian.niephaus@fokus.fraunhofer.de>

/** \file
    \brief WLANPacket unit tests */

#include "WLANPacket.hh"

// Custom includes
#include "WLANBeaconPacket.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
/* test parser with a data frame */
SENF_AUTO_TEST_CASE(WLANPacket_dataFrame_packet)
{
    unsigned char data[] = {
            0x88, 0x01, 0x00, 0x00, 0x00, 0x1a,             //header
            0x4d, 0x3e, 0xc7, 0x5c, 0x00, 0x0b, 0x6b, 0x57,
            0x06, 0xb0, 0x00, 0x18, 0x4d, 0x6e, 0x78, 0x48,
            0x30, 0x00, 0x01, 0x00,
            0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00,  //llc header
    };

    senf::WLANPacket_DataFrame p (senf::WLANPacket_DataFrame::create(data));

    BOOST_CHECK_EQUAL( p->version(),        0u    );
    BOOST_CHECK_EQUAL( p->type(),           2u    );
    BOOST_CHECK_EQUAL( p->subtype(),        8u    );
    BOOST_CHECK_EQUAL( p->toDS(),           true  );
    BOOST_CHECK_EQUAL( p->fromDS(),         false );
    BOOST_CHECK_EQUAL( p->moreFrag(),       false );
    BOOST_CHECK_EQUAL( p->retry(),          false );
    BOOST_CHECK_EQUAL( p->pwrMgt(),         false );
    BOOST_CHECK_EQUAL( p->moreData(),       false );
    BOOST_CHECK_EQUAL( p->protectedFrame(), false );
    BOOST_CHECK_EQUAL( p->order(),          false );

    BOOST_CHECK_EQUAL( p->duration(),       0u    );

    BOOST_CHECK_EQUAL( p->destinationAddress(),
            senf::MACAddress::from_string("00:18:4d:6e:78:48") );
    BOOST_CHECK_EQUAL( p->sourceAddress(),
                       senf::MACAddress::from_string("00:0b:6b:57:06:b0") );
    BOOST_CHECK_EQUAL (p->bssid().value(),
                       senf::MACAddress::from_string("00:1a:4d:3e:c7:5c") );

    BOOST_CHECK_EQUAL( p->bssid().value(),
                       p->receiverAddress().value() );
    BOOST_CHECK_EQUAL( p->transmitterAddress().value(),
                       p->sourceAddress().value() );

    BOOST_CHECK_EQUAL( p->sequenceNumber(), 3u );
    BOOST_CHECK_EQUAL( p->fragmentNumber(), 0u );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump(oss) );

    BOOST_REQUIRE( p.next(senf::nothrow) );
    BOOST_CHECK( p.next().is<senf::LlcSnapPacket>() );
}


/* test parser with creating a data frame*/
SENF_AUTO_TEST_CASE(WLANPacket_dataFrame_create)
{
    senf::WLANPacket_DataFrame p (senf::WLANPacket_DataFrame::create());

    SENF_CHECK_NO_THROW( p->bssid() = senf::MACAddress::from_string("00:1a:4d:3e:c7:5c") );
    SENF_CHECK_NO_THROW( p->sequenceNumber(4095u));

    BOOST_CHECK_EQUAL( p->type(), 2u );
    BOOST_CHECK_EQUAL( p->sequenceNumber(), 4095u );
}


/* test parser with a beacon frame */
SENF_AUTO_TEST_CASE(WLANPacket_beaconFrame_packet)
{
    unsigned char data[] = {
            0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0x00, 0x1a, 0x4d, 0x3e, 0xc7, 0x5c,
            0x00, 0x1a, 0x4d, 0x3e, 0xc7, 0x5c, 0xe0, 0x12,
            0x38, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00,
            0x64, 0x00, 0x01, 0x05, 0x00, 0x05, 0x62, 0x6f,
            0x78, 0x43, 0x31, 0x01, 0x08, 0x8c, 0x12, 0x98,
            0x24, 0xb0, 0x48, 0x60, 0x6c, 0x03, 0x01, 0x40,
            0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x07, 0x2a,
            0x4e, 0x41, 0x49, 0x24, 0x01, 0x11, 0x28, 0x01,
            0x11, 0x2c, 0x01, 0x11, 0x30, 0x01, 0x11, 0x34,
            0x01, 0x17, 0x38, 0x01, 0x17, 0x3c, 0x01, 0x17,
            0x40, 0x01, 0x17, 0x95, 0x01, 0x1e, 0x99, 0x01,
            0x1e, 0x9d, 0x01, 0x1e, 0xa1, 0x01, 0x1e, 0xa5,
            0x01, 0x1e, 0x20, 0x01, 0x00, 0xdd, 0x18, 0x00,
            0x50, 0xf2, 0x02, 0x01, 0x01, 0x81, 0x00, 0x02,
            0xa3, 0x40, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42,
            0x43, 0x5e, 0x00, 0x62, 0x32, 0x2f, 0x00, 0xa0,
            0x00, 0xdd, 0x09, 0x00, 0x03, 0x7f, 0x01, 0x01,
            0x00, 0x24, 0xff, 0x7f,
    };

    senf::WLANPacket_MgtFrame p (senf::WLANPacket_MgtFrame::create(data));

    BOOST_CHECK_EQUAL( p->version(),        0u    );
    BOOST_CHECK_EQUAL( p->type(),           0u    );
    BOOST_CHECK_EQUAL( p->subtype(),        8u    );
    BOOST_CHECK_EQUAL( p->toDS(),           false );
    BOOST_CHECK_EQUAL( p->fromDS(),         false );
    BOOST_CHECK_EQUAL( p->moreFrag(),       false );
    BOOST_CHECK_EQUAL( p->retry(),          false );
    BOOST_CHECK_EQUAL( p->pwrMgt(),         false );
    BOOST_CHECK_EQUAL( p->moreData(),       false );
    BOOST_CHECK_EQUAL( p->protectedFrame(), false );
    BOOST_CHECK_EQUAL( p->order(),          false );
    BOOST_CHECK_EQUAL( p->duration(),       0u    );

    BOOST_CHECK_EQUAL( p->destinationAddress(),
                       senf::MACAddress::from_string("ff:ff:ff:ff:ff:ff") );
    BOOST_CHECK_EQUAL( p->sourceAddress(),
                       senf::MACAddress::from_string("00:1a:4d:3e:c7:5c") );
    BOOST_CHECK_EQUAL( p->bssid(),
                       senf::MACAddress::from_string("00:1a:4d:3e:c7:5c") );

    BOOST_CHECK_EQUAL( p->sequenceNumber(), 302u );
    BOOST_CHECK_EQUAL( p->fragmentNumber(), 0u   );

    BOOST_CHECK( p.next().is<senf::WLANBeaconPacket>() );
}


/* test parser with creating a beacon frame*/
SENF_AUTO_TEST_CASE(WLANPacket_mgtFrame_create)
{
    senf::WLANPacket_MgtFrame p (senf::WLANPacket_MgtFrame::create());

    SENF_CHECK_NO_THROW(
            p->destinationAddress() = senf::MACAddress::from_string("ff:ff:ff:ff:ff:ff")
    );
    SENF_CHECK_NO_THROW(
            p->sourceAddress() = senf::MACAddress::from_string("00:1a:4d:3e:c7:5c")
    );
    SENF_CHECK_NO_THROW(
            p->bssid() = senf::MACAddress::from_string("00:1a:4d:3e:c7:5c")
    );
    SENF_CHECK_NO_THROW(
            p->sequenceNumber(2);
            p->sequenceNumber(p->sequenceNumber()+1);
    );

    BOOST_CHECK_EQUAL( p->type(), 0u );
    BOOST_CHECK_EQUAL( p->sequenceNumber(), 3u );
}


/* test parser with an ACK frame*/
SENF_AUTO_TEST_CASE(WLANPacket_ctrlFrame_packet)
{
    unsigned char data[] = {
            0xd4, 0x00, 0x00, 0x00,
            0x00, 0x0b, 0x6b, 0x57, 0x06, 0xb0,
            0x4f, 0xda, // What is this ?
    };

    senf::WLANPacket_CtrlFrame p (senf::WLANPacket_CtrlFrame::create(data));

    BOOST_CHECK_EQUAL( p->version(),        0u    );
    BOOST_CHECK_EQUAL( p->type(),           1u    );
    BOOST_CHECK_EQUAL( p->subtype(),        13u   );
    BOOST_CHECK_EQUAL( p->toDS(),           false );
    BOOST_CHECK_EQUAL( p->fromDS(),         false );
    BOOST_CHECK_EQUAL( p->moreFrag(),       false );
    BOOST_CHECK_EQUAL( p->retry(),          false );
    BOOST_CHECK_EQUAL( p->pwrMgt(),         false );
    BOOST_CHECK_EQUAL( p->moreData(),       false );
    BOOST_CHECK_EQUAL( p->protectedFrame(), false );
    BOOST_CHECK_EQUAL( p->order(),          false );

    BOOST_CHECK_EQUAL( p->duration(),       0u    );

    BOOST_CHECK_EQUAL( p->receiverAddress().value(),
                       senf::MACAddress::from_string("00:0b:6b:57:06:b0") );
}

/* test parser with creating an ACK frame*/
SENF_AUTO_TEST_CASE(WLANPacket_ctrlFrame_create)
{
    senf::WLANPacket_CtrlFrame p (senf::WLANPacket_CtrlFrame::create()) ;
    p->receiverAddress() = senf::MACAddress::from_string("00:1a:4d:3e:c7:5c");
    p->set_ack();

    BOOST_CHECK_EQUAL( p->type(), 1u);
    BOOST_CHECK_EQUAL( p->subtype(), 13u);
}


SENF_AUTO_TEST_CASE(WLANPacket_mesh_dataFrame_packet)
{
    unsigned char data[] = {
    		/* IEEE 802.11 QoS Data, Flags: ......FT */
    		0x88, 0x03, 0x30, 0x00, 0x4c, 0x5e, 0x0c, 0x11,
			0x6d, 0x86, 0x4c, 0x5e, 0x0c, 0x11, 0x6d, 0x72,
			0x4c, 0x5e, 0x0c, 0x11, 0x6d, 0x86, 0x40, 0x01,
			0x4c, 0x5e, 0x0c, 0x11, 0x6d, 0x72, 0x00, 0x01,
			0x00, 0x1f, 0x1a, 0x4f, 0x00, 0x00,
			/* Logical-Link Control */
			0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00,
			/* Internet Protocol Version 4, */
			0x45, 0x00, 0x00, 0x54, 0x63, 0xf8, 0x40, 0x00,
			0x40, 0x01, 0x8d, 0x5c, 0xc0, 0xa8, 0x64, 0x02,
			0xc0, 0xa8, 0x64, 0x01,
			/* Internet Control Message Protocol */
			0x08, 0x00, 0x33, 0x50, 0x79, 0x40, 0x00, 0x14,
			0x99, 0xde, 0x45, 0x58, 0x76, 0x21, 0x0b, 0x00,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
			0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
			0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };

    senf::WLANPacket_DataFrame p (senf::WLANPacket_DataFrame::create(data));

    BOOST_CHECK_EQUAL( p->version(),    0u );
    BOOST_CHECK_EQUAL( p->type(),       2u );
    BOOST_CHECK_EQUAL( p->subtype(),    8u );
    BOOST_CHECK_EQUAL( p->duration(), 0x30 );

    BOOST_CHECK_EQUAL( p->receiverAddress(),
            senf::MACAddress::from_string("4c:5e:0c:11:6d:86") );
    BOOST_CHECK_EQUAL( p->transmitterAddress(),
    		senf::MACAddress::from_string("4c:5e:0c:11:6d:72") );
    BOOST_CHECK_EQUAL( p->destinationAddress(),
    		senf::MACAddress::from_string("4c:5e:0c:11:6d:86") );

    BOOST_CHECK_EQUAL( p->sequenceNumber(), 20u );
    BOOST_CHECK_EQUAL( p->fragmentNumber(),  0u );

    BOOST_CHECK_EQUAL( p->sourceAddress(),
    		senf::MACAddress::from_string("4c:5e:0c:11:6d:72") );

    BOOST_CHECK( p->has_qosField() );

    BOOST_CHECK( p->has_meshControlField() );
    BOOST_CHECK_EQUAL( p->meshControlField().ttl(), 0x1f );
    BOOST_CHECK_EQUAL( p->meshControlField().sequenceNumber(), 0x00004f1a );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump(oss) );

    BOOST_REQUIRE( p.next(senf::nothrow) );
    BOOST_REQUIRE( p.next().is<senf::LlcSnapPacket>() );

    senf::LlcSnapPacket llcSnap (p.next().as<senf::LlcSnapPacket>());
    BOOST_CHECK_EQUAL( llcSnap->dsap().value(),   0xaa );
    BOOST_CHECK_EQUAL( llcSnap->ssap().value(),   0xaa );
    BOOST_CHECK_EQUAL( llcSnap->type_length(),  0x0800 );
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
