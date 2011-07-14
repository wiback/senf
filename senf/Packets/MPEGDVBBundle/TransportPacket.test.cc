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
    \brief TransportPacket unit tests */

//#include "TransportPacket.test.hh"
//#include "TransportPacket.test.ih"

// Custom includes
#include "TransportPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

using namespace senf;

SENF_AUTO_UNIT_TEST(transportPacket_parse)
{
    // TransportStream-Packet containing a ULE encoded IPv6 ping packet,
    // captured with dvbsnoop
    unsigned char data[] = {
            // Transport Packet Header
            0x47, 0x41, 0x0f, 0x1e,
            // Payload Pointer
            0x00,
            // SNDU Header
            0x80, 0x4c, 0x86, 0xdd,
            // IPv6 Packet
            0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x11, 0x40,
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x48, 0x7c, 0x7e, 0xff, 0xfe, 0x23, 0x68, 0xaf,
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04,
            0x02, 0x8c, 0x02, 0x8c, 0x00, 0x20, 0x58, 0xe1,
            0x11, 0x05, 0x12, 0x67, 0x06, 0x2f, 0x01, 0x00,
            0x20, 0x01, 0x06, 0x38, 0x04, 0x09, 0x10, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00,
            // SNDU footer (crc-32)
            0x1d, 0xc4, 0xe8, 0xc1,
            // Transport Packet stuffing
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };

    TransportPacket p (TransportPacket::create(data));

    BOOST_CHECK_EQUAL(   p->sync_byte(),                TransportPacketType::SYNC_BYTE+0 );
    BOOST_CHECK(       ! p->transport_error_indicator()                                  );
    BOOST_CHECK(         p->pusi()                                                       );
    BOOST_CHECK(       ! p->transport_priority()                                         );
    BOOST_CHECK_EQUAL(   p->pid(),                      0x010fu                          );
    BOOST_CHECK_EQUAL(   p->transport_scrmbl_ctrl(),    0x0u                             );
    BOOST_CHECK_EQUAL(   p->adaptation_field_ctrl(),    0x1u                             );
    BOOST_CHECK_EQUAL(   p->continuity_counter(),       0x0eu                            );
    BOOST_CHECK_EQUAL(   p->pointer_field(),            0x0u                             );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}


SENF_AUTO_UNIT_TEST(transportPacket_create_with_pusi)
{
    TransportPacket ts_packet (TransportPacket::create());
    ts_packet->setPUSI(true);
    ts_packet->pid() = 0x010fu;
    ts_packet->continuity_counter() = 0x0eu;

    unsigned char payload_data[] = {  // see test above
            // SNDU Header
            0x80, 0x4c, 0x86, 0xdd,
            // IPv6 Packet
            0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x11, 0x40,
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x48, 0x7c, 0x7e, 0xff, 0xfe, 0x23, 0x68, 0xaf,
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04,
            0x02, 0x8c, 0x02, 0x8c, 0x00, 0x20, 0x58, 0xe1,
            0x11, 0x05, 0x12, 0x67, 0x06, 0x2f, 0x01, 0x00,
            0x20, 0x01, 0x06, 0x38, 0x04, 0x09, 0x10, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00,
            // SNDU footer (crc-32)
            0x1d, 0xc4, 0xe8, 0xc1
    };
    senf::DataPacket payload (senf::DataPacket::createAfter( ts_packet, payload_data));

    // TODO: add method for stuffing
    BOOST_CHECK( true );
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
