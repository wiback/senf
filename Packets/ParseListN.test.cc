// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ParseListN.test unit tests */

//#include "ParseListN.test.hh"
//#include "ParseListN.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket_Type : public senf::PacketTypeBase
    {};
    typedef senf::ConcretePacket<VoidPacket_Type> VoidPacket;
}

BOOST_AUTO_UNIT_TEST(parseListN)
{
    typedef senf::Parse_VectorN<senf::Parse_UInt16,senf::Parse_UInt8>::parser ParseVec;
    typedef senf::Parse_ListN<ParseVec,senf::Parse_UInt16>::parser ParseList;
    
    VoidPacket vp (VoidPacket::create(ParseList::init_bytes));

    {
        ParseList p (vp.data().begin(),&vp.data());
        p.init();
        BOOST_CHECK_EQUAL( p.size(), 0u );
        BOOST_CHECK_EQUAL( p.bytes(), 2u );
        BOOST_CHECK( p.empty() );
        BOOST_CHECK( p.begin() == p.end() );
    }

    {
#       define p ParseList(vp.data().begin(),&vp.data())

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.bytes(), 3u );
        BOOST_CHECK_EQUAL( p.size(), 1u );
        BOOST_CHECK_EQUAL( p.front().bytes(), 1u );
        BOOST_CHECK_EQUAL( p.front().size(), 0u );
        BOOST_CHECK_EQUAL( vp.data()[1], 0x01u );

        p.front().push_back(0x1234u);
        BOOST_CHECK_EQUAL( p.front().size(), 1u );
        BOOST_CHECK_EQUAL( p.front().bytes(), 3u );
        BOOST_CHECK_EQUAL( p.front()[0], 0x1234u );
        BOOST_CHECK_EQUAL( p.size(), 1u );
        BOOST_CHECK_EQUAL( p.bytes(), 5u );

        p.front().push_back(0x2345u);
        BOOST_CHECK_EQUAL( p.front().back(), 0x2345u );
        BOOST_CHECK_EQUAL( p.bytes(), 7u );

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.size(), 2u );
        BOOST_CHECK_EQUAL( p.bytes(), 8u );
        
        p.back().push_front(0x0123u);
        BOOST_CHECK_EQUAL( p.front().size(), 2u );
        BOOST_CHECK_EQUAL( p.back().size(), 1u );

        p.push_front_space(2u);
        BOOST_CHECK_EQUAL( p.size(), 4u );
        BOOST_CHECK_EQUAL( p.front().size(), 0u);
        
        p.resize(3u);
        BOOST_CHECK_EQUAL( p.size(), 3u );
        BOOST_CHECK_EQUAL( p.back()[0], 0x1234u );
        BOOST_CHECK_EQUAL( p.bytes(), 9u );

#       undef p
    }
}

BOOST_AUTO_UNIT_TEST(parseListN_container)
{
    typedef senf::Parse_VectorN<senf::Parse_UInt16,senf::Parse_UInt8>::parser ParseVec;
    typedef senf::Parse_ListN<ParseVec,senf::Parse_UInt16>::parser ParseList;
    
    VoidPacket vp (VoidPacket::create(ParseList::init_bytes));

    ParseList(vp.data().begin(),&vp.data()).init();

    
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
