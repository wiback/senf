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

// Unit tests

// Custom includes
#include "RTPPacket.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>


BOOST_AUTO_UNIT_TEST(RTPPacket_packet)
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
