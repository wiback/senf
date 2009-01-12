// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIHPacket unit tests */

//#include "MIHPacket.test.hh"
//#include "MIHPacket.test.ih"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#include "MIHPacket.hh"
#include "senf/Utils/hexdump.hh"

using namespace senf;

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(MIHPacket_create)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket.finalizeThis();
    
//    mihPacket.dump(std::cout);
//    senf::hexdump(mihPacket.data().begin(), mihPacket.data().end(), std::cout);
    
    unsigned char data[] = { 
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04,
            0x01, 0x00,
            0x02, 0x00
    };
    BOOST_CHECK( equal( mihPacket.data().begin(), mihPacket.data().end(), data ));
}

///////////////////////////////cc.e////////////////////////////////////////
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
