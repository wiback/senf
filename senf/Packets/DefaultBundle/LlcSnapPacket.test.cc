//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief LlcSnapPacket unit tests */

#include "LlcSnapPacket.hh"

// Custom includes
#include "EthernetPacket.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(llcsnap_parse)
{
    senf::PacketData::byte data[] = {
        0xaa,             // DSAP
        0xaa,             // SSAP
        0x03,             // ctrl
        0x00, 0x00, 0x00, // Protocol Identification Field
        0x10, 0x11        // EtherType / Length
    };
    senf::LlcSnapPacket p (senf::LlcSnapPacket::create(data));

    BOOST_CHECK_EQUAL( p->dsap(), 0xaa );
    BOOST_CHECK_EQUAL( p->ssap(), 0xaa );
    BOOST_CHECK_EQUAL( p->ctrl(), 0x03 );
    BOOST_CHECK_EQUAL( p->protocolId(), 0x000000u );
    BOOST_CHECK_EQUAL( p->type_length(), 0x1011 );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

SENF_AUTO_TEST_CASE(llcSnapPacket_ethernet)
{
    senf::LlcSnapPacket llcsnap (senf::LlcSnapPacket::create());
    senf::EthernetPacket eth (senf::EthernetPacket::createAfter(llcsnap));
    senf::DataPacket payload  (senf::DataPacket::createAfter(
            eth, std::string("Hello, world!")));
    llcsnap.finalizeAll();

    BOOST_CHECK_EQUAL( llcsnap->dsap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ssap(), 0xaa );
    BOOST_CHECK_EQUAL( llcsnap->ctrl(), 0x03 );
    BOOST_CHECK_EQUAL( llcsnap->protocolId(), 0x000000u );
    // the type/length field was set to the ethertype 0x6558
    // (Trans Ether Bridging [RFC1701]) on finalize()
    BOOST_CHECK_EQUAL( llcsnap->type_length(), 0x6558);
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
