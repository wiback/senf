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

//#include "IpV4Packet.test.hh"
//#include "IpV4Packet.test.ih"

// Custom includes
#include "IpV4Packet.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::pkf;

BOOST_AUTO_UNIT_TEST(ipV4Packet_parser)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 
			     0x05, 0x06, 0x07, 0x08, 
			     0x09, 0x0A, 0x0B, 0x0C,   
                             0x11, 0x12, 0x13, 0x14, 
			     0x15, 0x16, 0x17, 0x18
                           };                        

    typedef unsigned char * iterator;
    Parse_IpV4<iterator> p(data);

    BOOST_CHECK_EQUAL( p.version(),     0x00u       );
    BOOST_CHECK_EQUAL( p.ihl(),         0x01u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.tos()), 0x02u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.length()), 0x0304u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.identifier()), 0x0506u );
    BOOST_CHECK_EQUAL( p.reserved(),    0           );
    BOOST_CHECK_EQUAL( p.df(),          0           );
    BOOST_CHECK_EQUAL( p.mf(),          0           );
    BOOST_CHECK_EQUAL( p.frag(),        0x0708u     );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.ttl()), 0x09u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.protocol()), 0x0Au );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.crc()), 0x0B0Cu );
    BOOST_CHECK_EQUAL( p.source(),      0x11121314u );
    BOOST_CHECK_EQUAL( p.destination(), 0x15161718u );
    
}

		      
BOOST_AUTO_UNIT_TEST(ipV4Packet_packet)
{

    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 
			     0x05, 0x06, 0x07, 0x08, 
			     0x09, 0x0A, 0x0B, 0x0C,   
                             0x11, 0x12, 0x13, 0x14, 
			     0x15, 0x16, 0x17, 0x18
                           };  

    IpV4Packet::ptr p (Packet::create<IpV4Packet>(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->version(),     0x00u       );
    BOOST_CHECK_EQUAL( p->ihl(),         0x01u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->tos()), 0x02u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->length()), 0x0304u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->identifier()), 0x0506u );
    BOOST_CHECK_EQUAL( p->reserved(),    0           );
    BOOST_CHECK_EQUAL( p->df(),          0           );
    BOOST_CHECK_EQUAL( p->mf(),          0           );
    BOOST_CHECK_EQUAL( p->frag(),        0x0708u     );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->ttl()), 0x09u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->protocol()), 0x0Au );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->crc()), 0x0B0Cu );
    BOOST_CHECK_EQUAL( p->source(),      0x11121314u );
    BOOST_CHECK_EQUAL( p->destination(), 0x15161718u );


}



///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
