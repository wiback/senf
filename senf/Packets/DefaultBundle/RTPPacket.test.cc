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
    \brief RTPPacket unit tests */

#include "RTPPacket.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(RTPPacket_packet)
{
    unsigned char data[] = {0x80 , 0x08 , 0x1b , 0xbb , 0x02 , 0xcb , 0xad , 0x80 , 0x50 , 0x48 , 0xa7, 0x8c };

    senf::RTPPacket p (senf::RTPPacket::create(data));

    BOOST_CHECK_EQUAL( p->version(), 2u);
    BOOST_CHECK_EQUAL( p->padding(), false);
    BOOST_CHECK_EQUAL( p->extension(), false);
    BOOST_CHECK_EQUAL( p->csrcCount(), 0u);
    BOOST_CHECK_EQUAL( p->marker(), false);
    BOOST_CHECK_EQUAL( p->payloadType(), 8u);
    BOOST_CHECK_EQUAL( p->seqNumber(), 0x1bbbu);
    BOOST_CHECK_EQUAL( p->timeStamp(), 0x2cbad80u);
    BOOST_CHECK_EQUAL( p->synSourceId(), 0x5048a78cu);

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
