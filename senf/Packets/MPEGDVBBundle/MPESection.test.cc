//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief MPESection unit tests */

#include "MPESection.hh"

// Custom includes
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

SENF_AUTO_TEST_CASE(MPESection_parse_chain)
{
    // MPE section captured with dvbsnoop
    unsigned char data[] = {
            // MPE Header
            0x3e, 0xb0, 0x77, 0x00, 0x00, 0xc3, 0x00, 0x00,
            0x04, 0xd8, 0x12, 0x0c,
            // LLC/SNAP Header
            0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x00, 0x62,
            // Ethernet Header
            0x01, 0x00, 0x5e, 0x01, 0x02, 0x03, 0x12, 0xb0,
            0x43, 0x61, 0x5d, 0x99, 0x08, 0x00,
            // IPv4 Header
            0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00,
            0x01, 0x01, 0xc6, 0xfb, 0xc0, 0xa8, 0x01, 0x01,
            0xef, 0x01, 0x02, 0x03,
            // Payload (ICMP)
            0x08, 0x00, 0x0e, 0xa3, 0xf2, 0x72, 0x55, 0xea,
            0xa2, 0xae, 0x56, 0x47, 0xbb, 0x06, 0x02, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            // MPE Trailer (crc)
            0xb5, 0x77, 0x4c, 0x3c
    };

    MPESection sec (MPESection::create(data));

    BOOST_CHECK_EQUAL(   sec->table_id(),                0x3eu       );
    BOOST_CHECK(         sec->section_syntax_indicator()             );
    BOOST_CHECK(       ! sec->private_indicator()                    );
    BOOST_CHECK_EQUAL(   sec->section_length(),          0x77u       );
    BOOST_CHECK_EQUAL(   sec->mac_addr_6(),              0x0u        );
    BOOST_CHECK_EQUAL(   sec->mac_addr_5(),              0x0u        );
    BOOST_CHECK_EQUAL(   sec->payload_scrmbl_ctrl(),     0x0u        );
    BOOST_CHECK_EQUAL(   sec->addr_scrmbl_ctrl(),        0x0u        );
    BOOST_CHECK      (   sec->llc_snap_flag()                        );
    BOOST_CHECK      (   sec->curr_next_indicator()                  );
    BOOST_CHECK_EQUAL(   sec->section_num(),             0x0u        );
    BOOST_CHECK_EQUAL(   sec->last_section_num(),        0x0u        );
    BOOST_CHECK_EQUAL(   sec->real_time_parameters().delta_t(),        0x04du  );
    BOOST_CHECK_EQUAL(   sec->real_time_parameters().table_boundary(), 1       );
    BOOST_CHECK_EQUAL(   sec->real_time_parameters().frame_boundary(), 0       );
    BOOST_CHECK_EQUAL(   sec->real_time_parameters().address(),        0x120cu );
    BOOST_CHECK_EQUAL(   sec->crc(), 3044494396u                     );
    BOOST_CHECK_EQUAL(   sec->crc(), sec->calcCrc()                  );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( sec.dump( oss));

    BOOST_REQUIRE( sec.next().is<LlcSnapPacket>() );
    LlcSnapPacket llcsnap (sec.next().as<LlcSnapPacket>());

    BOOST_CHECK_EQUAL( llcsnap->dsap(),        0xaau );
    BOOST_CHECK_EQUAL( llcsnap->ssap(),        0xaau );
    BOOST_CHECK_EQUAL( llcsnap->ctrl(),        0x3u  );
    BOOST_CHECK_EQUAL( llcsnap->protocolId(),  0x0u  );
    // in the captured section the llcsnap type/length field was set to the
    // length of the following ethernet packet; the ethertype 0x6558 (Trans
    // Ether Bridging [RFC1701]) would be possible as well (see next test)
    BOOST_CHECK_EQUAL( llcsnap->type_length(), 0x62u );

    BOOST_REQUIRE( llcsnap.next().is<EthernetPacket>() );
    EthernetPacket eth (llcsnap.next().as<EthernetPacket>());

    BOOST_CHECK_EQUAL( eth->destination().value(), MACAddress::from_string("01:00:5e:01:02:03") );
    BOOST_CHECK_EQUAL( eth->source().value(),      MACAddress::from_string("12:b0:43:61:5d:99") );
    BOOST_CHECK_EQUAL( eth->type_length(),         0x800u                                             );

    BOOST_REQUIRE( eth.next().is<IPv4Packet>() );
    IPv4Packet ip (eth.next().as<IPv4Packet>());

    BOOST_CHECK_EQUAL( ip->version(),    0x4u   );
    BOOST_CHECK_EQUAL( ip->ihl(),        0x5u   );
    BOOST_CHECK_EQUAL( ip->dscp(),       0x0u   );
    BOOST_CHECK_EQUAL( ip->length(),     0x54u  );
    BOOST_CHECK_EQUAL( ip->identifier(), 0x0u   );
    BOOST_CHECK_EQUAL( ip->df(),         1      );
    BOOST_CHECK_EQUAL( ip->mf(),         0      );
    BOOST_CHECK_EQUAL( ip->frag(),       0x0u   );
    BOOST_CHECK_EQUAL( ip->ttl(),        1      );
    BOOST_CHECK_EQUAL( ip->protocol(),   1      );
    BOOST_CHECK_EQUAL( ip->checksum(),   0xc6fb );
    BOOST_CHECK_EQUAL( ip->source().value(),      INet4Address::from_string("192.168.1.1") );
    BOOST_CHECK_EQUAL( ip->destination().value(), INet4Address::from_string("239.1.2.3")   );
}


SENF_AUTO_TEST_CASE(MPESection_create)
{
    MPESection sec (MPESection::create());
    sec->real_time_parameters().delta_t()        = 0x027u;
    sec->real_time_parameters().table_boundary() = 1;
    sec->real_time_parameters().frame_boundary() = 0;
    sec->real_time_parameters().address()        = 0xfffffu;

    // the type/length field will be set to the ethertype 0x6558 (Trans
    // Ether Bridging [RFC1701]) on finalize()
    LlcSnapPacket llcsnap (LlcSnapPacket::createAfter(sec));

    EthernetPacket eth (EthernetPacket::createAfter(llcsnap));
    eth->destination() = MACAddress::from_string("01:00:5e:01:02:03");
    eth->source()      = MACAddress::from_string("92:4c:a2:1c:da:81");

    IPv4Packet ip (IPv4Packet::createAfter(eth));
    ip->df()          = 1;
    ip->ttl()         = 1;
    ip->protocol()    = 1;
    ip->source()      = INet4Address::from_string("10.1.2.2");
    ip->destination() = INet4Address::from_string("239.1.2.3");

    unsigned char payload_data[] = {
            // Payload (ICMP)
            0x08, 0x00, 0x52, 0x73, 0x0e, 0x02, 0x00, 0x20,
            0xa4, 0x3a, 0xb4, 0x47, 0x4f, 0xe5, 0x04, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
    DataPacket payload (DataPacket::createAfter(ip, payload_data));

    sec.finalizeAll();

    unsigned char sec_data[] = {
            0x3e, 0xb0, 0x77, 0x00, 0x00, 0xc3, 0x00, 0x00,
            0x02, 0x7b, 0xff, 0xff, 0xaa, 0xaa, 0x03, 0x00,
            0x00, 0x00, 0x65, 0x58, 0x01, 0x00, 0x5e, 0x01,
            0x02, 0x03, 0x92, 0x4c, 0xa2, 0x1c, 0xda, 0x81,
            0x08, 0x00, 0x45, 0x00, 0x00, 0x54, 0x00, 0x00,
            0x40, 0x00, 0x01, 0x01, 0x7c, 0xa2, 0x0a, 0x01,
            0x02, 0x02, 0xef, 0x01, 0x02, 0x03, 0x08, 0x00,
            0x52, 0x73, 0x0e, 0x02, 0x00, 0x20, 0xa4, 0x3a,
            0xb4, 0x47, 0x4f, 0xe5, 0x04, 0x00, 0x08, 0x09,
            0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
            0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
            0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
            0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x52, 0xdf,
            0x6a, 0x1d
    };
    SENF_CHECK_EQUAL_COLLECTIONS(
            sec_data, sec_data+sizeof(sec_data), sec.data().begin(), sec.data().end() );
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
