// $Id: TransportPacket.test.cc 389 2007-08-10 15:06:54Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(tlvPacket_parse_packet_with_extended_length)
{
    unsigned char data[] = { 
        0x01, 0x23, 0x45, 0x67, // type
        0x81, // first and last bit set => one byte length following
        0x0A, // length (10 bytes value)
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A  // value (payload)
    };
            
    senf::TLVPacket p (senf::TLVPacket::create(data));
    
#include <iostream>
    p.dump(std::cout);

    BOOST_CHECK_EQUAL( p->type(), 0x01234567u );
    BOOST_CHECK_EQUAL( p->length(), 0x0Au );

    senf::PacketData & p_value (p.next().data());
//    BOOST_CHECK_EQUAL( p_value.size(), 0x0Au);
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
