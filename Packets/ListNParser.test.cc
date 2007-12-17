// $Id$
//
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
    \brief ListNParser.test unit tests */

//#include "ListNParser.test.hh"
//#include "ListNParser.test.ih"

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

    struct MyVec : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( size, senf::UInt8Parser );
        SENF_PARSER_VEC_N( vec, size, senf::UInt16Parser );
        
        SENF_PARSER_FINALIZE(MyVec);
    };
}

BOOST_AUTO_UNIT_TEST(ListNParser)
{
    typedef senf::ListNParser<MyVec,senf::UInt16Parser>::parser MyListNParser;
    
    VoidPacket vp (VoidPacket::create(MyListNParser::init_bytes));

    {
        MyListNParser p (vp.data().begin(),&vp.data());
        p.init();
        BOOST_CHECK_EQUAL( p.size(), 0u );
        BOOST_CHECK_EQUAL( p.bytes(), 2u );
        BOOST_CHECK( p.empty() );
        BOOST_CHECK( p.begin() == p.end() );
    }

    {
#       define p MyListNParser(vp.data().begin(),&vp.data())

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.bytes(), 3u );
        BOOST_CHECK_EQUAL( p.size(), 1u );
        BOOST_CHECK_EQUAL( p.front().bytes(), 1u );
        BOOST_CHECK_EQUAL( p.front().vec().size(), 0u );
        BOOST_CHECK_EQUAL( vp.data()[1], 0x01u );

        p.front().vec().push_back(0x1234u);
        BOOST_CHECK_EQUAL( p.front().vec().size(), 1u );
        BOOST_CHECK_EQUAL( p.front().bytes(), 3u );
        BOOST_CHECK_EQUAL( p.front().vec()[0], 0x1234u );
        BOOST_CHECK_EQUAL( p.size(), 1u );
        BOOST_CHECK_EQUAL( p.bytes(), 5u );

        p.front().vec().push_back(0x2345u);
        BOOST_CHECK_EQUAL( p.front().vec().back(), 0x2345u );
        BOOST_CHECK_EQUAL( p.bytes(), 7u );

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.size(), 2u );
        BOOST_CHECK_EQUAL( p.bytes(), 8u );
        
        p.back().vec().push_front(0x0123u);
        BOOST_CHECK_EQUAL( p.front().vec().size(), 2u );
        BOOST_CHECK_EQUAL( p.back().vec().size(), 1u );

        p.push_front_space(2u);
        BOOST_CHECK_EQUAL( p.size(), 4u );
        BOOST_CHECK_EQUAL( p.front().vec().size(), 0u);
        
        p.resize(3u);
        BOOST_CHECK_EQUAL( p.size(), 3u );
        BOOST_CHECK_EQUAL( p.back().vec()[0], 0x1234u );
        BOOST_CHECK_EQUAL( p.bytes(), 9u );

#       undef p
    }
}

BOOST_AUTO_UNIT_TEST(ListNParser_container)
{
    typedef senf::ListNParser<MyVec,senf::UInt16Parser>::parser MyListNParser;
    
    VoidPacket vp (VoidPacket::create(MyListNParser::init_bytes));

    MyListNParser(vp.data().begin(),&vp.data()).init();

    
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
