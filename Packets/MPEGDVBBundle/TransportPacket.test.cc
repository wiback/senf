// $Id: IpV4Packet.test.cc 364 2007-07-31 09:39:56Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

//#include "TransportPacket.test.hh"
//#include "TransportPacket.test.ih"

// Custom includes
#include "TransportPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(transportPacket_packet)
{
    // TransportStream-Packet containing a MPE encoded section with an IPv6 ping packet,
    // captured with dvbsnoop
    unsigned char data[] = { 
            0x47, 0x41, 0x0f, 0x1e, 0x00, 0x80, 0x4c, 0x86,
            0xdd, 0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x11, 
            0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x48, 0x7c, 0x7e, 0xff, 0xfe, 0x23, 0x68,
            0xaf, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
            0x04 ,0x02, 0x8c, 0x02, 0x8c, 0x00, 0x20, 0x58,
            0xe1, 0x11, 0x05, 0x12, 0x67, 0x06, 0x2f, 0x01,
            0x00, 0x20, 0x01, 0x06, 0x38, 0x04, 0x09, 0x10,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
            0x00, 0x1d, 0xc4, 0xe8, 0xc1, 0xff, 0xff, 0xff,
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
            0xff, 0xff, 0xff, 0xff
    };
            
    senf::TransportPacket p (senf::TransportPacket::create(data));

    BOOST_CHECK_EQUAL(   p->sync_byte(),                TRANSPORT_PACKET_SYNC_BYTE );
    BOOST_CHECK(       ! p->transport_error_indicator()                            );
    BOOST_CHECK(         p->pusi()                                                 );
    BOOST_CHECK(       ! p->transport_priority()                                   );
    BOOST_CHECK_EQUAL(   p->pid(),                      0x010fu                    );  
    BOOST_CHECK_EQUAL(   p->transport_scrmbl_ctrl(),    0x0u                       );             
    BOOST_CHECK_EQUAL(   p->adaptation_field_ctrl(),    0x1u                       );
    BOOST_CHECK_EQUAL(   p->continuity_counter(),       0x0eu                      );  
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
