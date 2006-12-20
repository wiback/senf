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

//#include "RTPPacket.test.hh"
//#include "RTPPacket.test.ih"

// Custom includes
#include "RTPPacket.hh"

#include "EthernetPacket.hh"
#include "IpV4Packet.hh"
#include "UDPPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(rtpPacket_parser)
{
    unsigned char data[] = { 0x13, 0x02, 0x03, 0x04, 
                             0x05, 0x06, 0x07, 0x08, 
                             0x09, 0x0A, 0x0B, 0x0C,

                             0x11, 0x12, 0x13, 0x14, // CSRC 1
                             0x15, 0x16, 0x17, 0x18, // CSRC 2
                             0x19, 0x1A, 0x1B, 0x1C // CSRC 3 
                             
                           };                       

    typedef unsigned char * iterator;
    Parse_RTP<iterator> p(data);

    BOOST_CHECK_EQUAL( p.version(),            0x00u       );
    BOOST_CHECK_EQUAL( p.padding(),            0           );
    BOOST_CHECK_EQUAL( p.extension(),          1           );
    BOOST_CHECK_EQUAL( p.csrcCount(),          0x03u       );    
    BOOST_CHECK_EQUAL( p.marker(),             0           );
    BOOST_CHECK_EQUAL( p.payloadType(),        0x02u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.seqNumber()), 0x0304u );
    BOOST_CHECK_EQUAL( p.timestamp(),          0x05060708u );
    BOOST_CHECK_EQUAL( p.ssrc(),               0x090A0B0Cu );
 
    BOOST_CHECK_EQUAL( p.csrcList()[0],        0x11121314u ); 
    BOOST_CHECK_EQUAL( p.csrcList()[1],        0x15161718u );
    BOOST_CHECK_EQUAL( p.csrcList()[2],        0x191A1B1Cu );


}

		      
BOOST_AUTO_UNIT_TEST(rtpPacket_packet)
{
    unsigned char data[] = { 0x33, 0x02, 0x03, 0x04, 
                             0x05, 0x06, 0x07, 0x08, 
                             0x09, 0x0A, 0x0B, 0x0C,

                             0x11, 0x12, 0x13, 0x14, // CSRC 1
                             0x15, 0x16, 0x17, 0x18, // CSRC 2
                             0x19, 0x1A, 0x1B, 0x1C, // CSRC 3 
                             
                             0x20, 0x21, 0x00, 0x04, // ex
                             0x24, 0x25, 0x26, 0x27, // ex
                             
                             0x20, 0x21, 0x08, 0x23, // paylaod
                             0x20, 0x00, 0x00, 0x03  // payload
                             
                           };  

    RTPPacket::ptr p (Packet::create<RTPPacket>(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->version(),            0x00u       );
    BOOST_CHECK_EQUAL( p->padding(),            1           );
    BOOST_CHECK_EQUAL( p->extension(),          1           );
    BOOST_CHECK_EQUAL( p->csrcCount(),          0x03u       );    
    BOOST_CHECK_EQUAL( p->marker(),             0           );
    BOOST_CHECK_EQUAL( p->payloadType(),        0x02u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->seqNumber()), 0x0304u );
    BOOST_CHECK_EQUAL( p->timestamp(),          0x05060708u );
    BOOST_CHECK_EQUAL( p->ssrc(),               0x090A0B0Cu );

    BOOST_CHECK_EQUAL( p->paddingOctet(),       3           );

    BOOST_CHECK_EQUAL( p->csrcList()[0],        0x11121314u ); 
    BOOST_CHECK_EQUAL( p->csrcList()[1],        0x15161718u );
    BOOST_CHECK_EQUAL( p->csrcList()[2],        0x191A1B1Cu );
    
    BOOST_REQUIRE( p->next() );
    BOOST_CHECK( p->next()->is<RTPUnknownExtensionPacket>() );

    RTPUnknownExtensionPacket::ptr v (p->next()->as<RTPUnknownExtensionPacket>());
    BOOST_CHECK_EQUAL( static_cast<unsigned>(v->proDef()), 0x2021u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(v->length()), 0x04u   );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(v->ext()[0]), 0x24u   );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(v->ext()[2]), 0x26u   );
    
    BOOST_REQUIRE( v->next() );
    DataPacket::ptr d (v->next()->as<DataPacket>());
    
    BOOST_CHECK_EQUAL( d->size(),               5u           );
   
}



BOOST_AUTO_UNIT_TEST(eth_rtpPacket_packet)
{
    unsigned char data[] = { 

                             // Ethernet
                             0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // destination MAC
                             0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  // source MAC
                             0x08, 0x00,                          // EtherType: IPv4 0x08

                             // IPv4
                             0x01, 0x02, 0x03, 0x04, 
			     0x05, 0x06, 0x07, 0x08, 
			     0x09, 0x11, 0x0B, 0x0C,              // EtherType: UDP 0x11  
                             0x11, 0x12, 0x13, 0x14, 
			     0x15, 0x16, 0x17, 0x18,

                             // UDP 
                             0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,

                             // RTP
                             0x33, 0x02, 0x03, 0x04, 
                             0x05, 0x06, 0x07, 0x08, 
                             0x09, 0x0A, 0x0B, 0x0C,

                             0x11, 0x12, 0x13, 0x14, // CSRC 1
                             0x15, 0x16, 0x17, 0x18, // CSRC 2
                             0x19, 0x1A, 0x1B, 0x1C, // CSRC 3 
                             
                             0x20, 0x21, 0x00, 0x04, // ex
                             0x24, 0x25, 0x26, 0x27, // ex
                             
                             0x20, 0x21, 0x08, 0x23, // paylaod
                             0x20, 0x00, 0x00, 0x03  // payload
                             
                           };  

    // Ethernet
    EthernetPacket::ptr p (Packet::create<EthernetPacket>(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->destination()[3], 0x04 );
    BOOST_CHECK_EQUAL( p->source()[0], 0x07 );
    BOOST_CHECK_EQUAL( p->type(), 0x0800 );

    // IP
    BOOST_REQUIRE( p->next() );
    BOOST_CHECK( p->next()->is<IpV4Packet>() );

    IpV4Packet::ptr ip (p->next()->as<IpV4Packet>());

    BOOST_CHECK_EQUAL( static_cast<unsigned>(ip->protocol()), 0x11u );

    // UDP
    BOOST_REQUIRE( ip->next() );
    BOOST_CHECK( ip->next()->is<UDPPacket>() );

    UDPPacket::ptr udp (ip->next()->as<UDPPacket>());

    BOOST_CHECK_EQUAL( udp->source(),            0x0102       );
    BOOST_CHECK_EQUAL( udp->destination(),       0x0304       );
    BOOST_CHECK_EQUAL( udp->length(),            0x0506       );
    BOOST_CHECK_EQUAL( udp->crc(),               0x0708       ); 


    // RTP
    BOOST_REQUIRE( udp->next() );
    BOOST_CHECK( udp->next()->is<DataPacket>() );

    RTPPacket::ptr rtp (udp->next()->reinterpret<RTPPacket>());

    BOOST_CHECK_EQUAL( rtp->version(),            0x00u       );
    BOOST_CHECK_EQUAL( rtp->padding(),            1           );
    BOOST_CHECK_EQUAL( rtp->extension(),          1           );
    BOOST_CHECK_EQUAL( rtp->csrcCount(),          0x03u       );    
    BOOST_CHECK_EQUAL( rtp->marker(),             0           );
    BOOST_CHECK_EQUAL( rtp->payloadType(),        0x02u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(rtp->seqNumber()), 0x0304u );
    BOOST_CHECK_EQUAL( rtp->timestamp(),          0x05060708u );
    BOOST_CHECK_EQUAL( rtp->ssrc(),               0x090A0B0Cu );

    BOOST_CHECK_EQUAL( rtp->paddingOctet(),       3           );

    BOOST_CHECK_EQUAL( rtp->csrcList()[0],        0x11121314u ); 
    BOOST_CHECK_EQUAL( rtp->csrcList()[1],        0x15161718u );
    BOOST_CHECK_EQUAL( rtp->csrcList()[2],        0x191A1B1Cu );
    
    BOOST_REQUIRE( rtp->next() );
    BOOST_CHECK( rtp->next()->is<RTPUnknownExtensionPacket>() );

    RTPUnknownExtensionPacket::ptr ex (rtp->next()->as<RTPUnknownExtensionPacket>());
    BOOST_CHECK_EQUAL( static_cast<unsigned>(ex->proDef()), 0x2021u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(ex->length()), 0x04u   );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(ex->ext()[0]), 0x24u   );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(ex->ext()[2]), 0x26u   );
    
    BOOST_REQUIRE( ex->next() );
    DataPacket::ptr pay (ex->next()->as<DataPacket>());
    
    BOOST_CHECK_EQUAL( pay->size(),               5u          );
   
}





///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
