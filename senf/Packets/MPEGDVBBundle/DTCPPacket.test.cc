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


/** \file
    \brief DTCPPacket unit tests */

#include "DTCPPacket.hh"

// Custom includes
#include <senf/Utils/String.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(dtcpPacket)
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
