// $Id$
//
// Copyright (C) 2008
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief DTCPPacket unit tests */

//#include "DTCPPacket.test.hh"
//#include "DTCPPacket.test.ih"

// Custom includes
#include "DTCPPacket.hh"
#include <senf/Utils/String.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(dtcpPacket)
{
    unsigned char data[] = {
            0x11,                 // versionNumber = 1, command = JOIN
            5,                    // interval
            0x0A, 0x0B,           // sequence number
            0x14,                 // receiveCapable = true, ipVersion = 4
            23,                   // tunnelProtocol
            2,                    // fbipCount
            0x00,
            101, 102, 103, 104,   // fbip1
            201, 202, 203, 204    // fbip2
    };

    senf::DTCPHelloPacket hello (senf::DTCPHelloPacket::create(
                                     boost::make_iterator_range(data, data+sizeof(data))));

    BOOST_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
                                   hello.data().begin(), hello.data().end() );

    BOOST_CHECK_EQUAL( senf::bytes(hello.parser()), 16u );

    BOOST_CHECK_EQUAL( hello->versionNumber(), 1u );
    BOOST_CHECK_EQUAL( hello->command(), unsigned(senf::DTCPHelloPacket::Parser::JOIN) );
    BOOST_CHECK_EQUAL( hello->interval(), 5u );
    BOOST_CHECK_EQUAL( hello->sequenceNumber(), 0x0A0Bu );
    BOOST_CHECK_EQUAL( hello->receiveCapableFeed(), true );
    BOOST_CHECK_EQUAL( hello->ipVersion(), 4u );
    BOOST_CHECK_EQUAL( hello->tunnelProtocol(), 23u );
    BOOST_CHECK_EQUAL( hello->fbipCount(), 2u );
    BOOST_CHECK( hello->has_v4fbipList() );
    BOOST_CHECK_EQUAL( senf::str(hello->v4fbipList()[0]), "101.102.103.104" );
    BOOST_CHECK_EQUAL( senf::str(hello->v4fbipList()[1]), "201.202.203.204" );

    std::stringstream ss;
    hello.dump(ss);
    BOOST_CHECK_EQUAL( ss.str(),
                       "DTCP HELLO Packet:\n"
                       "  version                 : 1\n"
                       "  command                 : JOIN\n"
                       "  interval                : 5\n"
                       "  sequence number         : 2571\n"
                       "  receive capable feed    : yes\n"
                       "  ip version              : 4\n"
                       "  tunnel protocol         : 23\n"
                       "  number of BDL ips       : 2\n"
                       "  feed BDL ips:\n"
                       "    101.102.103.104\n"
                       "    201.202.203.204\n" );

    senf::DTCPHelloPacket hello2 (senf::DTCPHelloPacket::create());

    SENF_CHECK_NO_THROW( hello2->versionNumber() = 1u );
    SENF_CHECK_NO_THROW( hello2->command() = senf::DTCPHelloPacket::Parser::JOIN );
    SENF_CHECK_NO_THROW( hello2->interval() = 5u );
    SENF_CHECK_NO_THROW( hello2->sequenceNumber() = 0x0A0B );
    SENF_CHECK_NO_THROW( hello2->receiveCapableFeed() = true );
    SENF_CHECK_NO_THROW( hello2->tunnelProtocol() = 23u );
    SENF_CHECK_NO_THROW( hello2->init_v4fbipList() );
    SENF_CHECK_NO_THROW( hello2->v4fbipList().push_back( senf::INet4Address(0x65666768u) ) );
    SENF_CHECK_NO_THROW( hello2->v4fbipList().push_back( senf::INet4Address(0xC9CACBCCu) ) );

    BOOST_CHECK_EQUAL( senf::bytes(hello2.parser()), 16u );
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
                                   hello2.data().begin(), hello2.data().end() );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
