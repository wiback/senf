// $Id$
//
// Copyright (C) 2006
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
//   Philipp Batroff <pug@berlios.de>

// Definition of non-inline non-template functions

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include <senf/Packets/DataPacket.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>


SENF_AUTO_UNIT_TEST(ICMPv6_MLDv2_Packet_packet)
{
    unsigned char data[] = {
        0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00,
        0xff, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16
    };

    senf::MLDv2ListenerReport p ( senf::MLDv2ListenerReport::create(data) );
    BOOST_CHECK_EQUAL(p->reserved(),0x0000 );
    typedef senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container_type recContainer_t;
    recContainer_t mcastAddrRecords (p->mcastAddrRecords());
    recContainer_t::iterator mcAddrIt (mcastAddrRecords.begin() );
    BOOST_CHECK_EQUAL( mcAddrIt->recordType(), 0x04);
    BOOST_CHECK_EQUAL( mcAddrIt->mcAddress().value(), senf::INet6Address::from_string("ff15::16"));
}
