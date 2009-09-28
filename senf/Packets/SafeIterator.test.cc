// $Id$
//
// Copyright (C) 2008
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
    \brief SafeIterator unit tests */

//#include "SafeIterator.test.hh"
//#include "SafeIterator.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};
}

BOOST_AUTO_UNIT_TEST(safePacketParser)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(6u)));
    senf::SafePacketParserWrapper<senf::UInt16Parser> p;

    BOOST_CHECK( !p );

    p =  senf::UInt16Parser(pi->data().begin(),&pi->data());

    BOOST_CHECK( p );
    (*p) = 0x1234u;

    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    BOOST_CHECK_EQUAL( p->data()[0], 0x12u );

    p->data().resize(1024u);
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    (*p) = 0x2345u;
    BOOST_CHECK_EQUAL( p->data()[0], 0x23u );
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
