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
    \brief ICMPv6Packet unit tests */

#include "ICMPv6Packet.hh"

// Custom includes
#include <sstream>
#include "ICMPv6TypePacket.hh"
#include "NDPOptions.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(ICMPv6Packet_packet)
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

    unsigned char dataRouterSolicitation[] = {
        0x85, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x05, 0x01, 0x00, 0x00,
        0x12, 0x34, 0x56, 0x78
    };

    senf::ICMPv6Packet pRouterSolicitation ( senf::ICMPv6Packet::create(dataRouterSolicitation) );
    BOOST_CHECK_EQUAL( pRouterSolicitation->type(),     0x85   );
    BOOST_CHECK_EQUAL( pRouterSolicitation->code(),     0x00   );
    BOOST_CHECK_EQUAL( pRouterSolicitation->checksum(), 0x0000 );
    BOOST_CHECK( pRouterSolicitation.next() );
    BOOST_CHECK( pRouterSolicitation.next().is<senf::NDPRouterSolicitationMessage>() );
    BOOST_CHECK_EQUAL( pRouterSolicitation.next().size(), 12u );

    senf::NDPRouterSolicitationMessage pOption(pRouterSolicitation.next().as<senf::NDPRouterSolicitationMessage>());
    senf::NDPRouterSolicitationMessage::Parser::options_t::container_type optC(pOption->options() );
    senf::NDPRouterSolicitationMessage::Parser::options_t::container_type::iterator listIter (optC.begin());
    BOOST_CHECK_EQUAL(listIter->type(),5u);
    BOOST_CHECK_EQUAL(listIter->length(),1u);

    SENF_CHECK_NO_THROW( pRouterSolicitation.dump( oss));

    unsigned char dataRouterAdvertisement[] = {
        0x86, 0x00, 0x00, 0x00,
        0xFF, 0x00, 0x23, 0x28,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    senf::ICMPv6Packet pRouterAdvertisement ( senf::ICMPv6Packet::create(dataRouterAdvertisement) );
    BOOST_CHECK_EQUAL( pRouterAdvertisement->type(),     0x86   );
    BOOST_CHECK_EQUAL( pRouterAdvertisement->code(),     0x00   );
    BOOST_CHECK_EQUAL( pRouterAdvertisement->checksum(), 0x0000 );
    BOOST_CHECK( pRouterAdvertisement.next() );
    BOOST_CHECK( pRouterAdvertisement.next().is<senf::NDPRouterAdvertisementMessage>() );
    BOOST_CHECK_EQUAL( pRouterAdvertisement.next().size(), 12u );

    SENF_CHECK_NO_THROW( pRouterAdvertisement.dump( oss));

    unsigned char dataNeighborSolicitation[] = {
        0x87, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    senf::ICMPv6Packet pNeighborSolicitation ( senf::ICMPv6Packet::create(dataNeighborSolicitation) );
    BOOST_CHECK_EQUAL( pNeighborSolicitation->type(),     0x87   );
    BOOST_CHECK_EQUAL( pNeighborSolicitation->code(),     0x00   );
    BOOST_CHECK_EQUAL( pNeighborSolicitation->checksum(), 0x0000 );
    BOOST_CHECK( pNeighborSolicitation.next() );
    BOOST_CHECK( pNeighborSolicitation.next().is<senf::NDPNeighborSolicitationMessage>() );
    BOOST_CHECK_EQUAL( pNeighborSolicitation.next().size(), 20u );

    SENF_CHECK_NO_THROW( pNeighborSolicitation.dump( oss));

    unsigned char dataNeighborAdvertisement[] = {
        0x88, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    senf::ICMPv6Packet pNeighborAdvertisement ( senf::ICMPv6Packet::create(dataNeighborAdvertisement) );
    BOOST_CHECK_EQUAL( pNeighborAdvertisement->type(),     0x88   );
    BOOST_CHECK_EQUAL( pNeighborAdvertisement->code(),     0x00   );
    BOOST_CHECK_EQUAL( pNeighborAdvertisement->checksum(), 0x0000 );
    BOOST_CHECK( pNeighborAdvertisement.next() );
    BOOST_CHECK( pNeighborAdvertisement.next().is<senf::NDPNeighborAdvertisementMessage>() );
    BOOST_CHECK_EQUAL( pNeighborAdvertisement.next().size(), 20u );

    SENF_CHECK_NO_THROW( pNeighborAdvertisement.dump( oss));

    unsigned char dataRedirect[] = {
        0x89, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    senf::ICMPv6Packet pRedirect ( senf::ICMPv6Packet::create(dataRedirect) );
    BOOST_CHECK_EQUAL( pRedirect->type(),     0x89   );
    BOOST_CHECK_EQUAL( pRedirect->code(),     0x00   );
    BOOST_CHECK_EQUAL( pRedirect->checksum(), 0x0000 );
    BOOST_CHECK( pRedirect.next() );
    BOOST_CHECK( pRedirect.next().is<senf::NDPRedirectMessage>() );
    BOOST_CHECK_EQUAL( pRedirect.next().size(), 36u );

    SENF_CHECK_NO_THROW( pRedirect.dump( oss));
}

SENF_AUTO_TEST_CASE(ICMPv6Packet_create)
{
    std::ostringstream oss (std::ostringstream::out);

    unsigned char ping[] = { 0x60, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3a, 0x40,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                             0x80, 0x00, 0xda, 0xe0, 0x9f, 0x7e, 0x00, 0x09,

                             0xb7, 0x3c, 0xbb, 0x4a, 0x9d, 0x90, 0x0a, 0x00, //payload
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                             0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                             0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                             0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                             0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                             0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };

    senf::IPv6Packet ip (senf::IPv6Packet::create());
    ip->hopLimit() = 64;
    ip->source() = senf::INet6Address::Loopback;
    ip->destination() = senf::INet6Address::Loopback;

    senf::ICMPv6Packet icmp (senf::ICMPv6Packet::createAfter(ip));
    icmp->code() = 0;

    senf::ICMPv6EchoRequest ereq (senf::ICMPv6EchoRequest::createAfter(icmp));
    ereq->identifier() = 0x9f7e;
    ereq->seqNr() = 9;

    senf::DataPacket data (
        senf::DataPacket::createAfter(ereq, std::make_pair(ping+48, ping+sizeof(ping))));

    ip.finalizeAll();

    SENF_CHECK_NO_THROW (ip.dump( oss ));

    std::string dump (
        "Internet protocol Version 6:\n"
        "  version                 : 6\n"
        "  traffic class           : 0x00\n"
        "  flow label              : 0x00000\n"
        "  payload length          : 64\n"
        "  next header             : 58\n"
        "  hop limit               : 64\n"
        "  source                  : ::1\n"
        "  destination             : ::1\n"
        "ICMPv6 protocol:\n"
        "  type                    : 128\n"
        "  code                    : 0\n"
        "  checksum                : 0xdae0\n"
        "ICMPv6 Echo Request:\n"
        "  Identifier              : 40830\n"
        "  SequenceNumber          : 9\n"
        "Payload data (56 bytes)\n"
        );

    {
        std::stringstream ss;
        ip.dump(ss);
        BOOST_CHECK_EQUAL( ss.str(), dump );
    }

    SENF_CHECK_EQUAL_COLLECTIONS( ip.data().begin(), ip.data().end(),
                                  ping, ping+sizeof(ping) );

    senf::IPv6Packet orig (senf::IPv6Packet::create(ping));

    {
        std::stringstream ss;
        orig.dump(ss);
        BOOST_CHECK_EQUAL( ss.str(), dump );
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
