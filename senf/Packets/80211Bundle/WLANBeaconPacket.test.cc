// $Id: WLANPacket.test.cc 1317 2009-08-21 14:07:47Z g0dil $
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief WLANPacket unit tests */

// Custom includes
#include "WLANBeaconPacket.hh"
#include <senf/Packets/Packets.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

///////////////////////////////cc.p////////////////////////////////////////
/* test parser with a data frame */
BOOST_AUTO_UNIT_TEST(WLANBeaconPacket_packet)
{
    unsigned char data[] = {
        0x3a, 0x30, 0xaa, 0x4c, 0x9c, 0x00, 0x00, 0x00, //timestamp
        0x64, 0x00, //beacon interval
        0x01, 0x05, //capability information
        0x00, 0x05, 0x62, 0x6f, 0x78, 0x43, 0x31, //SSID
        0x01, 0x08, 0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c, //supported rates
        0x03, 0x01, 0x40, //DS parameter set
        0x05, 0x04, 0x00, 0x01, 0x00, 0x00, //TIM
        0x07, 0x2a, 0x4e, 0x41, 0x49, 0x24, 0x01, 0x11, 0x28, 0x01, 0x11,
        0x2c, 0x01, 0x11, 0x30, 0x01, 0x11, 0x34, 0x01, 0x17, 0x38, 0x01,
        0x17, 0x3c, 0x01, 0x17, 0x40, 0x01, 0x17, 0x95, 0x01, 0x1e, 0x99,
        0x01, 0x1e, 0x9d, 0x01, 0x1e, 0xa1, 0x01, 0x1e, 0xa5, 0x01, 0x1e, //Country information
        0x20, 0x01, 0x00, //power contraint
        0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x88, 0x00, 0x02,
        0xa3, 0x00, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42, 0x43, 0x5e, 0x00,
        0x62, 0x32, 0x2f, 0x00,  //vendor specific
    };
    senf::WLANBeaconPacket p (senf::WLANBeaconPacket::create(data));

    BOOST_CHECK_EQUAL( p->timestamp(), 0x0000009C4CAA303AuLL);
    BOOST_CHECK_EQUAL( p->beaconInterval(), 100u);

 
}


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
