// $Id: main.test.cc 206 2008-06-08 14:20:52Z pug $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <Philipp.Batroff@fokus.fraunhofer.de>
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

#include "ICMPv6Packet.hh"
#include "../../Packets/DataPacket.hh"
#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include "ICMPv6TypePacket.hh"

BOOST_AUTO_UNIT_TEST(ICMPv6_MLDv2_Packet_packet)
{
    unsigned char data[] = {0x00 ,0x00 ,0x00 ,0x01 ,0x04 ,0x00 ,0x00 ,0x00 ,0xff ,0x15 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x16};


    senf::MLDv2ListenerReport p ( senf::MLDv2ListenerReport::create(data) );
    BOOST_CHECK_EQUAL(p->reserved(),0x0000 );    
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container mcastAddrRecords (p->mcastAddrRecords());
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container::iterator mcAddrIt (mcastAddrRecords.begin() );
    BOOST_CHECK_EQUAL(mcAddrIt->recordType(), 0x04);
    BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(mcAddrIt->mcAddress() ), "ff15::16");
}
