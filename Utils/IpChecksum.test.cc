// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief IpChecksum.test unit tests */

//#include "IpChecksum.test.hh"
//#include "IpChecksum.test.ih"

// Custom includes
#include "IpChecksum.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(ipChecksum)
{
    char data[] = { 0x45, 0x00, 0x00, 0x28, 0x49, 0x44, 0x40, 0x00,
                    0x40, 0x06, 0x00, 0x00, 0x0a, 0xc1, 0x01, 0x06,
                    0xc2, 0x9f, 0xa4, 0xc3 };

    senf::IpChecksum summer;
    summer.feed(data, data+sizeof(data));
    BOOST_CHECK_EQUAL( summer.sum(), 0x7e62u );
}


///////////////////////////////cc.e////////////////////////////////////////
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
