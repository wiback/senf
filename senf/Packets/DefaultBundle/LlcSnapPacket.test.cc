// $Id$
//
// Copyright (C) 2007
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief LlcSnapPacket unit tests */

//#include "LlcSnapPacket.test.hh"
//#include "LlcSnapPacket.test.ih"

// Custom includes
#include "LlcSnapPacket.hh"
#include "EthernetPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(llcsnap_parse)
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

SENF_AUTO_UNIT_TEST(llcSnapPacket_ethernet)
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
