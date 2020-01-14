//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"
#include <senf/Packets/DataPacket.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>


SENF_AUTO_TEST_CASE(ICMPv6_MLDv2_Packet_packet)
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
