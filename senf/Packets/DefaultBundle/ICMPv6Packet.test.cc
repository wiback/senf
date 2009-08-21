// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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
    \brief ICMPv6Packet unit tests */

// Custom includes
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>


BOOST_AUTO_UNIT_TEST(ICMPv6Packet_packet)
{
    unsigned char dataListenerReport[] = {
            0x8f, 0x00, 0x8d, 0x54, 0x00, 0x00, 0x00, 0x01,
            0x04, 0x00, 0x00, 0x00, 0xff, 0x15, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x16
    };

    senf::ICMPv6Packet pListenerReport ( senf::ICMPv6Packet::create(dataListenerReport) );

    BOOST_CHECK_EQUAL( pListenerReport->type(),     0x8f   );
    BOOST_CHECK_EQUAL( pListenerReport->code(),     0x00   );
    BOOST_CHECK_EQUAL( pListenerReport->checksum(), 0x8d54 );
    BOOST_CHECK( pListenerReport.next() );
    BOOST_CHECK( pListenerReport.next().is<senf::MLDv2ListenerReport>() );
    BOOST_CHECK_EQUAL( pListenerReport.next().size(), 24u );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( pListenerReport.dump( oss));

    unsigned char dataListenerQuery[] = {
        0x82, 0x00, 0xf7, 0xd6, 0x27, 0x10, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x7d, 0x00, 0x00
    };
    senf::ICMPv6Packet pListenerQuery ( senf::ICMPv6Packet::create(dataListenerQuery) );
    BOOST_CHECK_EQUAL( pListenerQuery->type(),     0x82   );
    BOOST_CHECK_EQUAL( pListenerQuery->code(),     0x00   );
    BOOST_CHECK_EQUAL( pListenerQuery->checksum(), 0xf7d6 );
    BOOST_CHECK( pListenerQuery.next() );
    BOOST_CHECK( pListenerQuery.next().is<senf::MLDv2ListenerQuery>() );
    BOOST_CHECK_EQUAL( pListenerQuery.next().size(), 24u );

    SENF_CHECK_NO_THROW( pListenerQuery.dump( oss));
    
    unsigned char dataEchoRequest[] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07
    };
    senf::ICMPv6Packet pEchoRequest ( senf::ICMPv6Packet::create(dataEchoRequest) );
    BOOST_CHECK_EQUAL( pEchoRequest->type(),     0x80   );
    BOOST_CHECK_EQUAL( pEchoRequest->code(),     0x00   );
    BOOST_CHECK_EQUAL( pEchoRequest->checksum(), 0x0000 );
    BOOST_CHECK( pEchoRequest.next() );
    BOOST_CHECK( pEchoRequest.next().is<senf::ICMPv6EchoRequest>() );
    BOOST_CHECK_EQUAL( pEchoRequest.next().size(), 4u );

    SENF_CHECK_NO_THROW( pEchoRequest.dump( oss));
    
    unsigned char dataEchoReply[] = {
        0x81, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x4d
    };
    
    senf::ICMPv6Packet pEchoReply ( senf::ICMPv6Packet::create(dataEchoReply) );
    BOOST_CHECK_EQUAL( pEchoReply->type(),     0x81   );
    BOOST_CHECK_EQUAL( pEchoReply->code(),     0x00   );
    BOOST_CHECK_EQUAL( pEchoReply->checksum(), 0x0000 );
    BOOST_CHECK( pEchoReply.next() );
    BOOST_CHECK( pEchoReply.next().is<senf::ICMPv6EchoReply>() );
    BOOST_CHECK_EQUAL( pEchoReply.next().size(), 4u );

    SENF_CHECK_NO_THROW( pEchoReply.dump( oss));
    
    
    unsigned char dataErrDestUnreachable[] = {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };
    
    senf::ICMPv6Packet pErrDestUnreachable ( senf::ICMPv6Packet::create(dataErrDestUnreachable) );
    BOOST_CHECK_EQUAL( pErrDestUnreachable->type(),     0x01   );
    BOOST_CHECK_EQUAL( pErrDestUnreachable->code(),     0x00   );
    BOOST_CHECK_EQUAL( pErrDestUnreachable->checksum(), 0x0000 );
    BOOST_CHECK( pErrDestUnreachable.next() );
    BOOST_CHECK( pErrDestUnreachable.next().is<senf::ICMPv6ErrDestUnreachable>() );
    BOOST_CHECK_EQUAL( pErrDestUnreachable.next().size(), 4u );

    SENF_CHECK_NO_THROW( pErrDestUnreachable.dump( oss));
    
    
    unsigned char dataErrTooBig[] = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xd8 
    };
    
    senf::ICMPv6Packet pErrTooBig ( senf::ICMPv6Packet::create(dataErrTooBig) );
    BOOST_CHECK_EQUAL( pErrTooBig->type(),     0x02   );
    BOOST_CHECK_EQUAL( pErrTooBig->code(),     0x00   );
    BOOST_CHECK_EQUAL( pErrTooBig->checksum(), 0x0000 );
    BOOST_CHECK( pErrTooBig.next() );
    BOOST_CHECK( pErrTooBig.next().is<senf::ICMPv6ErrTooBig>() );
    BOOST_CHECK_EQUAL( pErrTooBig.next().size(), 4u );

    SENF_CHECK_NO_THROW( pErrTooBig.dump( oss));
    
    
    unsigned char dataErrTimeExceeded[] = {
        0x03, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    senf::ICMPv6Packet pErrTimeExceeded ( senf::ICMPv6Packet::create(dataErrTimeExceeded) );
    BOOST_CHECK_EQUAL( pErrTimeExceeded->type(),     0x03   );
    BOOST_CHECK_EQUAL( pErrTimeExceeded->code(),     0x63   );
    BOOST_CHECK_EQUAL( pErrTimeExceeded->checksum(), 0x0000 );
    BOOST_CHECK( pErrTimeExceeded.next() );
    BOOST_CHECK( pErrTimeExceeded.next().is<senf::ICMPv6ErrTimeExceeded>() );
    BOOST_CHECK_EQUAL( pErrTimeExceeded.next().size(), 4u );

    SENF_CHECK_NO_THROW( pErrTimeExceeded.dump( oss));
    
    
    unsigned char dataErrParamProblem[] = {
        0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    senf::ICMPv6Packet pErrParamProblem ( senf::ICMPv6Packet::create(dataErrParamProblem) );
    BOOST_CHECK_EQUAL( pErrParamProblem->type(),     0x04   );
    BOOST_CHECK_EQUAL( pErrParamProblem->code(),     0x01   );
    BOOST_CHECK_EQUAL( pErrParamProblem->checksum(), 0x0000 );
    BOOST_CHECK( pErrParamProblem.next() );
    BOOST_CHECK( pErrParamProblem.next().is<senf::ICMPv6ErrParamProblem>() );
    BOOST_CHECK_EQUAL( pErrParamProblem.next().size(), 4u );

    SENF_CHECK_NO_THROW( pErrParamProblem.dump( oss));
    
}

// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
