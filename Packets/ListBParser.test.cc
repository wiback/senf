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
    \brief ListBParser.test unit tests */

//#include "ListBParser.test.hh"
//#include "ListBParser.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    struct VectorParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( size, senf::UInt8Parser );
        SENF_PARSER_VEC_N( vec, size, senf::UInt16Parser );

        SENF_PARSER_FINALIZE(VectorParser);
    };

    typedef senf::ListBParser<VectorParser,senf::UInt16Parser>::parser MyListBParser;
}

BOOST_AUTO_UNIT_TEST(ListBParser)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            MyListBParser::init_bytes));
    
    MyListBParser p (pi->data().begin(),&pi->data());
    p.init();
    BOOST_CHECK_EQUAL( p.size(), 0u );
    BOOST_CHECK_EQUAL( p.bytes(), 2u );
    BOOST_CHECK( p.empty() );
    BOOST_CHECK( p.begin() == p.end() );

    // the mutators are really tested together with the container wrappers since they are based
    // on the container wrapper. Here we only need one call to make the list larger ...

    p.push_back_space();
    p = MyListBParser(pi->data().begin(),&pi->data());
    BOOST_CHECK_EQUAL( p.bytes(), 3u );
    BOOST_CHECK_EQUAL( p.size(), 1u );
    BOOST_CHECK( ! p.empty() );
    BOOST_CHECK( p.begin() != p.end() );
}

BOOST_AUTO_UNIT_TEST(ListBParser_container)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            MyListBParser::init_bytes));
    
    {
        MyListBParser::container c (MyListBParser(pi->data().begin(),&pi->data()));
     
        BOOST_CHECK_EQUAL( c.size(), 0u );
        BOOST_CHECK_EQUAL( c.bytes(), 2u );
        BOOST_CHECK( c.begin() == c.end() );
        
        c.shift(c.begin());
        BOOST_CHECK_EQUAL( c.size(), 1u );
        BOOST_CHECK_EQUAL( c.bytes(), 3u );

        BOOST_CHECK_EQUAL( c.front().vec().size(), 0u );
        c.front().vec().push_back(0x1234u);
        BOOST_CHECK_EQUAL( c.bytes(), 5u );

        {
            senf::PacketInterpreterBase::ptr pi2 (senf::PacketInterpreter<VoidPacket>::create(
                    MyListBParser::init_bytes));
            MyListBParser::container c2 (MyListBParser(pi2->data().begin(),&pi2->data()));
            c2.push_back_space();
            {
                VectorParser::vec_t::container c2v (c2.front().vec());
                c2v.push_back(0x2345u);
                c2v.push_back(0x3456u);
            }

            BOOST_CHECK_EQUAL(c2.size(), 1u);
            BOOST_CHECK_EQUAL(c2.bytes(), 7u);
            
            c.insert(c.end(),c2.back());
            BOOST_CHECK_EQUAL( c.size(), 2u );
            BOOST_CHECK_EQUAL( c.bytes(), 10u );
            BOOST_CHECK_EQUAL( c.back().vec()[0], 0x2345u );
            BOOST_CHECK_EQUAL( c.back().bytes(), c2.back().bytes() );

            c2.back().vec()[0] << 0x1357u;
            c.insert(boost::next(c.begin()), 2u, c2.back());
            BOOST_CHECK_EQUAL( c.size(), 4u );
            BOOST_CHECK_EQUAL( c.bytes(), 20u );
            BOOST_CHECK_EQUAL( (*boost::next(c.begin())).vec()[0], 0x1357u ); 
            BOOST_CHECK_EQUAL( (*boost::next(c.begin(),2)).vec()[0], 0x1357u );

            c2.back().vec()[0] << 0x2468u;
            c.insert(c.begin(),c2.begin(),c2.end());
            BOOST_CHECK_EQUAL( c.size(), 5u );
            BOOST_CHECK_EQUAL( c.bytes(), 25u );
            BOOST_CHECK_EQUAL( c.front().vec()[0], 0x2468u );

            c.erase(c.begin(),2);
            BOOST_CHECK_EQUAL( c.size(), 3u );
            BOOST_CHECK_EQUAL( c.bytes(), 17u );
            BOOST_CHECK_EQUAL( c.front().vec()[0],0x1357u );
            BOOST_CHECK_EQUAL( c.back().vec()[0], 0x2345u );
            
            c.erase((boost::next(c.begin(),2)),c.end());
            BOOST_CHECK_EQUAL( c.size(), 2u );
            BOOST_CHECK_EQUAL( c.bytes(), 12u );
            BOOST_CHECK_EQUAL( c.front().vec()[0],0x1357u );
            BOOST_CHECK_EQUAL( c.back().vec()[0], 0x1357u );

            c.clear();
            BOOST_CHECK_EQUAL( c.size(), 0u );
            BOOST_CHECK_EQUAL( c.bytes(), 2u );
       }
    }
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
