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

//#include "RTCPPacket.test.hh"
//#include "RTCPPacket.test.ih"

// Custom includes
#include "RTCPPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::pkf;

BOOST_AUTO_UNIT_TEST(rtcpPacket_parser)
{

    // RTCP RR (no RB)
    unsigned char data_1[] = { 
                             0x80, 0xc8, 0x00, 0x06, 
                             0xe5, 0x70, 0xaa, 0x18, 
                             0xc7, 0xc2, 0xb2, 0x00,
                             0xc3, 0xd7, 0x0e, 0x96,
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x01,
                             0x00, 0x00, 0x0d, 0xc8
				                             
                           };                       

    typedef unsigned char * iterator_1;
    Parse_RTCP<iterator_1> p_1(data_1);

    BOOST_CHECK_EQUAL( p_1.version(),            2u          );
    BOOST_CHECK_EQUAL( p_1.padding(),            0           );
    BOOST_CHECK_EQUAL( p_1.count(),              0u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_1.payloadType()), 200u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_1.length()), 0x0006u );

    BOOST_CHECK_EQUAL(p_1.rr().ssrc(),           0xe570aa18u );

    BOOST_CHECK_EQUAL(p_1.rr().ntp_msb(),        0xc7c2b200u );
    BOOST_CHECK_EQUAL(p_1.rr().ntp_lsb(),        0xc3d70e96u );
    BOOST_CHECK_EQUAL(p_1.rr().timestamp(),      0x00u       );
    BOOST_CHECK_EQUAL(p_1.rr().spcount(),        0x01u       );
    BOOST_CHECK_EQUAL(p_1.rr().socount(),        0x0dc8u     );


    // TODO RTCP RR
    unsigned char data_2[] = { 
                             0x82, 0xc8, 0x00, 0x06, 
                             0xe5, 0x70, 0xaa, 0x18, 
                             0xc7, 0xc2, 0xb2, 0x00,
                             0xc3, 0xd7, 0x0e, 0x96,
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x01,
                             0x00, 0x00, 0x0d, 0xc8,

                             0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x10, 0x11, 0x12,
                             0x13, 0x14, 0x15, 0x16,
                             0x20, 0x21, 0x22, 0x23,

                             0x99, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x10, 0x11, 0x12,
                             0x13, 0x14, 0x15, 0x16,
                             0x20, 0x21, 0x22, 0x23
				                             
                           };  

    typedef unsigned char * iterator_2;
    Parse_RTCP<iterator_2> p_2(data_2);

    BOOST_CHECK_EQUAL( p_2.version(),            2u          );
    BOOST_CHECK_EQUAL( p_2.padding(),            0           );
    BOOST_CHECK_EQUAL( p_2.count(),              2u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_2.payloadType()), 200u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_2.length()), 0x0006u );

    BOOST_CHECK_EQUAL(p_2.rr().ssrc(),           0xe570aa18u );

    BOOST_CHECK_EQUAL(p_2.rr().ntp_msb(),        0xc7c2b200u );
    BOOST_CHECK_EQUAL(p_2.rr().ntp_lsb(),        0xc3d70e96u );
    BOOST_CHECK_EQUAL(p_2.rr().timestamp(),      0x00u       );
    BOOST_CHECK_EQUAL(p_2.rr().spcount(),        0x01u       );
    BOOST_CHECK_EQUAL(p_2.rr().socount(),        0x0dc8u     );

    BOOST_CHECK_EQUAL( p_2.rr().rbVec().size(),    0x02u  );

 
    BOOST_CHECK_EQUAL( p_2.rr().rbVec().begin()->ssrc(),     0x01020304u  );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_2.rr().rbVec().begin()->fragLost()), 0x05u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_2.rr().rbVec().begin()->cnpl()[0]), 0x06u );
    BOOST_CHECK_EQUAL( p_2.rr().rbVec().begin()->ehsnr(),    0x09101112u  );
    BOOST_CHECK_EQUAL( p_2.rr().rbVec().begin()->LSR(),      0x13141516u  );
    BOOST_CHECK_EQUAL( p_2.rr().rbVec().begin()->DLSR(),     0x20212223u  );

    typedef unsigned char * iterator;
    typedef Parse_UIntField     < 3, 8, iterator >    Parse_Count;
    typedef Parse_Vector        < Parse_RTCP_RB<>, Parse_Count, iterator > Parse_rbVec;

    Parse_rbVec::iterator j_2 (p_2.rr().rbVec().begin());

    BOOST_CHECK_EQUAL( j_2->ssrc(),     0x01020304u  );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j_2->fragLost()), 0x05u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j_2->cnpl()[0]), 0x06u );
    BOOST_CHECK_EQUAL( j_2->ehsnr(),    0x09101112u  );
    BOOST_CHECK_EQUAL( j_2->LSR(),      0x13141516u  );
    BOOST_CHECK_EQUAL( j_2->DLSR(),     0x20212223u  );

    ++j_2;

    BOOST_CHECK_EQUAL( j_2->ssrc(),     0x99020304u  );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j_2->fragLost()), 0x05u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j_2->cnpl()[0]), 0x06u );
    BOOST_CHECK_EQUAL( j_2->ehsnr(),    0x09101112u  );
    BOOST_CHECK_EQUAL( j_2->LSR(),      0x13141516u  );
    BOOST_CHECK_EQUAL( j_2->DLSR(),     0x20212223u  );






    // RTCP SR
    unsigned char data_3[] = { 
                             0x82, 0xc9, 0x00, 0x06, 
                             0xe5, 0x70, 0xaa, 0x18, 
                             
                             0x99, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x10, 0x11, 0x12,
                             0x13, 0x14, 0x15, 0x16,
                             0x20, 0x21, 0x22, 0x23,

                             0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x10, 0x11, 0x12,
                             0x13, 0x14, 0x15, 0x16,
                             0x20, 0x21, 0x22, 0x23
				                             
                           };  

    typedef unsigned char * iterator_3;
    Parse_RTCP<iterator_3> p_3(data_3);

    BOOST_CHECK_EQUAL( p_3.version(),            2u          );
    BOOST_CHECK_EQUAL( p_3.padding(),            0           );
    BOOST_CHECK_EQUAL( p_3.count(),              2u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_3.payloadType()), 201u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_3.length()), 0x0006u );

    BOOST_CHECK_EQUAL( p_3.sr().ssrc(),           0xe570aa18u );
    BOOST_CHECK_EQUAL( p_3.sr().rbVec().size(),    0x02u  );

 
    typedef unsigned char * iterator;
    typedef Parse_UIntField     < 3, 8, iterator >    Parse_Count;
    typedef Parse_Vector        < Parse_RTCP_RB<>, Parse_Count, iterator > Parse_rbVec;

    Parse_rbVec::iterator j (p_3.rr().rbVec().begin());
    BOOST_CHECK_EQUAL( j->ssrc(),     0x01020304u  );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j->fragLost()), 0x05u );
    BOOST_CHECK_EQUAL( j->ehsnr(),    0x09101112u  );
    BOOST_CHECK_EQUAL( j->LSR(),      0x13141516u  );
    BOOST_CHECK_EQUAL( j->DLSR(),     0x20212223u  );

    ++j;

#if 0
    BOOST_CHECK_EQUAL( j->ssrc(),     0x99020304u  );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j->fragLost()), 0x05u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(j->cnpl()[0]), 0x06u );
    BOOST_CHECK_EQUAL( j->ehsnr(),    0x09101112u  );
    BOOST_CHECK_EQUAL( j->LSR(),      0x13141516u  );
    BOOST_CHECK_EQUAL( j->DLSR(),     0x20212223u  );
#endif

    // TODO RTCP SDES

    unsigned char data_4[] = { 
                             0x81, 0xca, 0x00, 0x04, 
                             0xe5, 0x70, 0xaa, 0x18, 
                             0x01, 0x09, 0x39, 0x30, 
                             0x31, 0x31, 0x33, 0x35, 
                             0x37, 0x36, 0x37, 0x00
				                             
                           }; 

    typedef unsigned char * iterator_4;
    Parse_RTCP<iterator_4> p_4(data_4); 

    BOOST_CHECK_EQUAL( p_4.version(),            2u          );
    BOOST_CHECK_EQUAL( p_4.padding(),            0           );
    BOOST_CHECK_EQUAL( p_4.count(),              1u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_4.payloadType()), 202u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_4.length()), 0x0004u );

    BOOST_CHECK_EQUAL( p_4.sdes().chunkVec().size(),    0x01u      );
#if 0
    Parse_RTCP_SDES::Parse_itemList::iterator j_4 (p_4.sdes().chunkVec().begin());

// TODO  -> ask Stefan

 // BOOST_CHECK_EQUAL( p_4.sdes().chunkList()[0].ssrc(), 0xe570aa18u);
 // BOOST_CHECK_EQUAL( p_4.sdes().chunkList()[0].itemList().size(), 0x01u);
 // BOOST_CHECK( p_4.sdes().chunkList()[0].itemList().check(data_4+20) );
 // BOOST_CHECK_EQUAL(p.sdes().chunkList()[0].chunkList().size(),  1);

//item 
//     typeField(), 0x01u
//     length(), 0x09u
//     desc(), 0x393031313335373637u

#endif


   // RTCP BYE 
   unsigned char data_5[] = { 
                             0x82, 0xcb, 0x00, 0x06, 
                                    
                             0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08
                             				                             
                           };  

    typedef unsigned char * iterator_5;
    Parse_RTCP<iterator_5> p_5(data_5);

    BOOST_CHECK_EQUAL( p_5.version(),            2u          );
    BOOST_CHECK_EQUAL( p_5.padding(),            0           );
    BOOST_CHECK_EQUAL( p_5.count(),              2u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_5.payloadType()), 203u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_5.length()), 0x0006u );

    BOOST_CHECK_EQUAL( p_5.bye().ssrcVec().size(),    0x02u  );

    BOOST_CHECK_EQUAL( p_5.bye().ssrcVec()[0],     0x01020304u  );
    BOOST_CHECK_EQUAL( p_5.bye().ssrcVec()[1],     0x05060708u  );


   // RTCP APP 
   unsigned char data_6[] = { 
                             0x82, 0x7b, 0x00, 0x05, 
                                    
                             0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x10, 0x11, 0x12,
                             0x00, 0x00, 0x00, 0x08
                             				                             
                           };  

    typedef unsigned char * iterator_6;
    Parse_RTCP<iterator_6> p_6(data_6);

    BOOST_CHECK_EQUAL( p_6.version(),              2u           );
    BOOST_CHECK_EQUAL( p_6.padding(),              0            );
    BOOST_CHECK_EQUAL( p_6.count(),                2u           );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_6.payloadType()), 123u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_6.length()), 0x0005u );

    BOOST_CHECK_EQUAL( p_6.app().ssrc(),           0x01020304u  );
    BOOST_CHECK_EQUAL( p_6.app().name(),           0x05060708u  );  
#if 0    
    BOOST_CHECK_EQUAL( p_6.app().appData().size(), 2u           );
#endif
    BOOST_CHECK_EQUAL( p_6.app().appData()[0],     0x09101112u  );
    BOOST_CHECK_EQUAL( p_6.app().appData()[1],     0x08u        );

}

		      
BOOST_AUTO_UNIT_TEST(rtcpPacket_packet)
{
#if 0
    unsigned char data_1[] = { 
                             0x80, 0xc8, 0x00, 0x06, 
                             0xe5, 0x70, 0xaa, 0x18, 
                             0xc7, 0xc2, 0xb2, 0x00,
                             0xc3, 0xd7, 0x0e, 0x96,
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x01,
                             0x00, 0x00, 0x0d, 0xc8
				                             
                           };   

    RTCPPacket::ptr p_1 (Packet::create<RTCPPacket>(data_1, data_1+sizeof(data_1)));

    BOOST_CHECK_EQUAL( p_1->version(),            2u          );
    BOOST_CHECK_EQUAL( p_1->padding(),            0           );
    BOOST_CHECK_EQUAL( p_1->count(),              0u          );    
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_1->payloadType()), 200u );
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p_1->length()), 0x0006u );

    BOOST_CHECK_EQUAL(p_1->rr().ssrc(),           0xe570aa18u );

    BOOST_CHECK_EQUAL(p_1->rr().ntp_msb(),        0xc7c2b200u );
    BOOST_CHECK_EQUAL(p_1->rr().ntp_lsb(),        0xc3d70e96u );
    BOOST_CHECK_EQUAL(p_1->rr().timestamp(),      0x00u       );
    BOOST_CHECK_EQUAL(p_1->rr().spcount(),        0x01u       );
    BOOST_CHECK_EQUAL(p_1->rr().socount(),        0x0dc8u     );
#endif
   
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
