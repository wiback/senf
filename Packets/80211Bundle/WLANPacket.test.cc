// $Id: main.test.cc 206 2008-08-06 14:20:52Z pug $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <christian.niephaus@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

// Custom includes

#include "../../Packets/Packets.hh"
#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include "WLANPacket.hh"

/* test parser with a data frame */
BOOST_AUTO_UNIT_TEST(WLANPacket_dataFrame_packet)
{


    unsigned char data[] = {0x88, 0x01, 0x00, 0x00, 0x00, 0x1a,             //header
                            0x4d, 0x3e, 0xc7, 0x5c, 0x00, 0x0b, 0x6b, 0x57,
                            0x06, 0xb0, 0x00, 0x18, 0x4d, 0x6e, 0x78, 0x48,
                            0x30, 0x00, 0x01, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //dummy data
                            0x38, 0x39, 0x30, 0x31                          //trailer
                            };




    senf::WLANPacket p (senf::WLANPacket::create(data));

    BOOST_CHECK_EQUAL( p->version(), 0u);
    BOOST_CHECK_EQUAL( p->type(), 2u);
    BOOST_CHECK_EQUAL( p->subtype(), 8u);
    BOOST_CHECK_EQUAL( p->toDS(), true);
    BOOST_CHECK_EQUAL( p->fromDS(), false);
    BOOST_CHECK_EQUAL( p->moreFrag(), false);
    BOOST_CHECK_EQUAL( p->retry(), false);
    BOOST_CHECK_EQUAL( p->pwrMgt(), false);
    BOOST_CHECK_EQUAL( p->moreData(), false);
    BOOST_CHECK_EQUAL( p->protectedFrame(), false);
    BOOST_CHECK_EQUAL( p->order(), false);

    BOOST_CHECK_EQUAL( p->duration(), 0u);

    BOOST_CHECK_EQUAL( p->dataFrame().da()[0], 0x00 );
    BOOST_CHECK_EQUAL( p->dataFrame().da()[1], 0x18 );
    BOOST_CHECK_EQUAL( p->dataFrame().da()[2], 0x4d );
    BOOST_CHECK_EQUAL( p->dataFrame().da()[3], 0x6e );
    BOOST_CHECK_EQUAL( p->dataFrame().da()[4], 0x78 );
    BOOST_CHECK_EQUAL( p->dataFrame().da()[5], 0x48 );

    BOOST_CHECK_EQUAL( p->dataFrame().sa()[0], 0x00 );
    BOOST_CHECK_EQUAL( p->dataFrame().sa()[3], 0x57 );
    BOOST_CHECK_EQUAL( p->dataFrame().sa()[5], 0xb0 );

    BOOST_CHECK_EQUAL( p->dataFrame().bssid()[0], 0x00 );
    BOOST_CHECK_EQUAL( p->dataFrame().bssid()[3], 0x3e );
    BOOST_CHECK_EQUAL( p->dataFrame().bssid()[5], 0x5c );

    BOOST_CHECK_EQUAL( p->dataFrame().bssid()[3],
                       p->dataFrame().ra()[3]);

    BOOST_CHECK_EQUAL( p->dataFrame().ta()[3],
                       p->dataFrame().sa()[3]);

    BOOST_CHECK_EQUAL( p->dataFrame().sequenceNumber(), 3u );
    BOOST_CHECK_EQUAL( p->dataFrame().fragmentNumber(), 0u );



}

/* test parser with creating a data frame*/
BOOST_AUTO_UNIT_TEST(WLANPacket_dataFrame_create)
{
    senf::WLANPacket p (senf::WLANPacket::create());

    SENF_CHECK_NO_THROW(p->init_dataFrame());
    p->dataFrame().bssid()=senf::MACAddress::from_string("00:1a:4d:3e:c7:5c");
    BOOST_CHECK_EQUAL( p->type(), 2u);
}



/* test parser with a beacon frame */
BOOST_AUTO_UNIT_TEST(WLANPacket_beaconFrame_packet)
{


    unsigned char data[] = {0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
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
                            0x00, 0x24, 0xff, 0x7f, 0x87, 0x07, 0x1f, 0xf5,
                            };

    senf::WLANPacket p (senf::WLANPacket::create(data));

    BOOST_CHECK_EQUAL( p->version(), 0u);
    BOOST_CHECK_EQUAL( p->type(), 0u);
    BOOST_CHECK_EQUAL( p->subtype(), 8u);
    BOOST_CHECK_EQUAL( p->toDS(), false);
    BOOST_CHECK_EQUAL( p->fromDS(), false);
    BOOST_CHECK_EQUAL( p->moreFrag(), false);
    BOOST_CHECK_EQUAL( p->retry(), false);
    BOOST_CHECK_EQUAL( p->pwrMgt(), false);
    BOOST_CHECK_EQUAL( p->moreData(), false);
    BOOST_CHECK_EQUAL( p->protectedFrame(), false);
    BOOST_CHECK_EQUAL( p->order(), false);

    BOOST_CHECK_EQUAL( p->duration(), 0u);

    BOOST_CHECK_EQUAL (p->has_mgtFrame(), true);

    BOOST_CHECK_EQUAL( p->mgtFrame().da()[0], 0xff );
    BOOST_CHECK_EQUAL( p->mgtFrame().da()[3], 0xff );
    BOOST_CHECK_EQUAL( p->mgtFrame().da()[5], 0xff );

    BOOST_CHECK_EQUAL( p->mgtFrame().sa()[0], 0x00 );
    BOOST_CHECK_EQUAL( p->mgtFrame().sa()[3], 0x3e );
    BOOST_CHECK_EQUAL( p->mgtFrame().sa()[5], 0x5c );

    BOOST_CHECK_EQUAL( p->mgtFrame().bssid()[0], 0x00 );
    BOOST_CHECK_EQUAL( p->mgtFrame().bssid()[3], 0x3e );
    BOOST_CHECK_EQUAL( p->mgtFrame().bssid()[5], 0x5c );

    BOOST_CHECK_EQUAL( p->mgtFrame().sequenceNumber(), 302u );
    BOOST_CHECK_EQUAL( p->mgtFrame().fragmentNumber(), 0u );


    BOOST_CHECK_EQUAL ( p->fcs(), 0x87071ff5);

}

/* test parser with creating a beacon frame*/
BOOST_AUTO_UNIT_TEST(WLANPacket_mgtFrame_create)
{
    senf::WLANPacket p (senf::WLANPacket::create());

    SENF_CHECK_NO_THROW(p->init_mgtFrame());
    SENF_CHECK_NO_THROW(
            p->mgtFrame().da()=senf::MACAddress::from_string("ff:ff:ff:ff:ff:ff")
    );
    SENF_CHECK_NO_THROW(
            p->mgtFrame().sa()=senf::MACAddress::from_string("00:1a:4d:3e:c7:5c")
    );
    SENF_CHECK_NO_THROW(
            p->mgtFrame().bssid()=senf::MACAddress::from_string("00:1a:4d:3e:c7:5c")
    );

    BOOST_CHECK_EQUAL( p->type(), 0u);
}

/* test parser with an ACK frame*/
BOOST_AUTO_UNIT_TEST(WLANPacket_ctrlFrame_packet)
{


    unsigned char data[] = {0xd4, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x6b, 0x57,
                            0x06, 0xb0, 0x4f, 0xda};

    senf::WLANPacket p (senf::WLANPacket::create(data));

    BOOST_CHECK_EQUAL( p->version(), 0u);
    BOOST_CHECK_EQUAL( p->type(), 1u);
    BOOST_CHECK_EQUAL( p->subtype(), 13u);
    BOOST_CHECK_EQUAL( p->toDS(), false);
    BOOST_CHECK_EQUAL( p->fromDS(), false);
    BOOST_CHECK_EQUAL( p->moreFrag(), false);
    BOOST_CHECK_EQUAL( p->retry(), false);
    BOOST_CHECK_EQUAL( p->pwrMgt(), false);
    BOOST_CHECK_EQUAL( p->moreData(), false);
    BOOST_CHECK_EQUAL( p->protectedFrame(), false);
    BOOST_CHECK_EQUAL( p->order(), false);

    BOOST_CHECK_EQUAL( p->duration(), 0u);

    BOOST_CHECK_EQUAL (p->ctrlFrame().ra()[0], 0x00);
    BOOST_CHECK_EQUAL (p->ctrlFrame().ra()[3], 0x57);
    BOOST_CHECK_EQUAL (p->ctrlFrame().ra()[5], 0xb0);
}

/* test parser with creating an ACK frame*/
BOOST_AUTO_UNIT_TEST(WLANPacket_ctrlFrame_create)
{
    senf::WLANPacket p (senf::WLANPacket::create()) ;
//    std::cout << unsigned( senf::init_bytes<senf::WLANPacketParser_CtrlFrameParser>::value ) << "\n";
    SENF_CHECK_NO_THROW(p->init_ctrlFrame());
//    std::cout << unsigned( senf::init_bytes<senf::WLANPacketParser_CtrlFrameParser>::value ) << "\n";
    p->ctrlFrame().ra()=senf::MACAddress::from_string("00:1a:4d:3e:c7:5c");
    p->ctrlFrame().set_ack();
    BOOST_CHECK_EQUAL( p->type(), 1u);
    BOOST_CHECK_EQUAL( p->subtype(), 13u);
}

