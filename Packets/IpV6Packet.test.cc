// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief IpV6Packet unit tests */

//#include "IpV6Packet.test.hh"
//#include "IpV6Packet.test.ih"

// Custom includes
#include "IpV6Packet.hh"
#include "Socket/INetAddressing.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

BOOST_AUTO_UNIT_TEST(ipV6Packet_parser)
{
    unsigned char data[] = { 0x60, 0x12, 0x20, 0x30,
			     0x01, 0x02, 0x03, 0x04,
			     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
			     0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f };

    typedef unsigned char * iterator;
    Parse_IpV6<iterator> p (data);

    BOOST_CHECK_EQUAL( unsigned(p.version()),       0x06u      );
    BOOST_CHECK_EQUAL( unsigned(p.trafficClass()),  0x01u      );
    BOOST_CHECK_EQUAL( unsigned(p.flowLabel()),     0x22030u   );
    BOOST_CHECK_EQUAL( unsigned(p.length()),        0x0102u    );
    BOOST_CHECK_EQUAL( unsigned(p.nextHeader()),    0x03u      );
    BOOST_CHECK_EQUAL( unsigned(p.hopLimit()),      0x04u      );
    BOOST_CHECK_EQUAL( INet6Address(p.source().range()).address() , 
		       "1011:1213:1415:1617:1819:1a1b:1c1d:1e1f" );
    BOOST_CHECK_EQUAL( INet6Address(p.destination().range()).address() , 
		       "2021:2223:2425:2627:2829:2a2b:2c2d:2e2f" );
}

BOOST_AUTO_UNIT_TEST(ipV6Packet_packet)
{
    unsigned char data[] = { 0x60, 0x12, 0x20, 0x30,
			     0x01, 0x02, 0x03, 0x04,
			     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
			     0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f };

    IpV6Packet::ptr p (Packet::create<IpV6Packet>(data, data+sizeof(data)));

    
    BOOST_CHECK_EQUAL( unsigned(p->version()),       0x06u      );
    BOOST_CHECK_EQUAL( unsigned(p->trafficClass()),  0x01u      );
    BOOST_CHECK_EQUAL( unsigned(p->flowLabel()),     0x22030u   );
    BOOST_CHECK_EQUAL( unsigned(p->length()),        0x0102u    );
    BOOST_CHECK_EQUAL( unsigned(p->nextHeader()),    0x03u      );
    BOOST_CHECK_EQUAL( unsigned(p->hopLimit()),      0x04u      );
    BOOST_CHECK_EQUAL( INet6Address(p->source().range()).address() , 
		       "1011:1213:1415:1617:1819:1a1b:1c1d:1e1f" );
    BOOST_CHECK_EQUAL( INet6Address(p->destination().range()).address() , 
		       "2021:2223:2425:2627:2829:2a2b:2c2d:2e2f" );

    BOOST_CHECK( p->next() );
    BOOST_CHECK( p->next()->is<DataPacket>() );
    BOOST_CHECK_EQUAL( p->next()->size(), 0u );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// ispell-local-dictionary: "american"
// End:
