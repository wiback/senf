// $Id$
//
// Copyright (C) 2006 
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

//#include "UDPPacket.test.hh"
//#include "UDPPacket.test.ih"

// Custom includes
#include "UDPPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(udpPacket_parser)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08
                           };                        

    typedef unsigned char * iterator;
    Parse_UDP<iterator> p(data);

    BOOST_CHECK_EQUAL( p.source(),            0x0102       );
    BOOST_CHECK_EQUAL( p.destination(),       0x0304       );
    BOOST_CHECK_EQUAL( p.length(),            0x0506       );
    BOOST_CHECK_EQUAL( p.crc(),               0x0708       );    
    
}

		      
BOOST_AUTO_UNIT_TEST(udpPacket_packet)
{

    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08
                           };   

    UDPPacket::ptr p (Packet::create<UDPPacket>(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->source(),            0x0102       );
    BOOST_CHECK_EQUAL( p->destination(),       0x0304       );
    BOOST_CHECK_EQUAL( p->length(),            0x0506       );
    BOOST_CHECK_EQUAL( p->crc(),               0x0708       );    

}



///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
