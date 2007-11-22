// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

//#include "ParseVec.test.hh"
//#include "ParseVec.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/assign.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};
}

BOOST_AUTO_UNIT_TEST(parseVec)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));
    typedef senf::Parse_Vector<
        senf::Parse_UInt16,
        senf::detail::Parse_VectorN_Sizer<senf::Parse_UInt8, 1u>
        > Parse_UInt16Vec;

    {
        Parse_UInt16Vec v (boost::next(p->data().begin(), 1), &p->data());
        
        BOOST_CHECK_EQUAL( v[0], 0x1011 );
        BOOST_CHECK_EQUAL( v[2], 0x1415 );
        BOOST_CHECK_EQUAL( v.size(), 3u );
        BOOST_CHECK_EQUAL( v.bytes(), 6u );
        BOOST_CHECK( ! v.empty() );
        p->data()[0] = 0x06;
        BOOST_CHECK_EQUAL( v.size(), 6u );
        BOOST_CHECK_EQUAL( v.bytes(), 12u );
        
        Parse_UInt16Vec::iterator b (v.begin());
        Parse_UInt16Vec::iterator e (v.end());
        BOOST_CHECK_EQUAL(std::distance(b,e), Parse_UInt16Vec::difference_type(v.size()));
    }

    // Warning: Each of the following operations invalidate the parser -> we need to recreate it at
    // each step

    // And since all these members directly call the corresponding members in the container wrapper,
    // we don't need to check them again below ...

    {
#       define v Parse_UInt16Vec(boost::next(p->data().begin(),1),&p->data())

        v.push_back(0xf0f1u,2);
        BOOST_CHECK_EQUAL( v.size(), 8u );
        BOOST_CHECK_EQUAL( v[7], 0xf0f1u );

        v.push_back_space();
        BOOST_CHECK_EQUAL( v.size(), 9u );
        BOOST_CHECK_EQUAL( v[8], 0u );
        
        v.push_front(0xf3f4u);
        BOOST_CHECK_EQUAL( v.size(), 10u );
        BOOST_CHECK_EQUAL( v[0], 0xf3f4u );
        BOOST_CHECK_EQUAL( v[1], 0x1011u );

        v.push_front_space(2);
        BOOST_CHECK_EQUAL( v.size(), 12u );
        BOOST_CHECK_EQUAL( v[0], 0u );
        BOOST_CHECK_EQUAL( v[1], 0u );
        BOOST_CHECK_EQUAL( v[2], 0xf3f4u );
        BOOST_CHECK_EQUAL( p->data().size(), 25u );

        v.resize(4);
        BOOST_CHECK_EQUAL( v.size(), 4u );
        BOOST_CHECK_EQUAL( p->data().size(), 9u );
        BOOST_CHECK_EQUAL( v[3], 0x1011u );

        v.resize(6);
        BOOST_CHECK_EQUAL( v.size(), 6u );
        BOOST_CHECK_EQUAL( v[5], 0u );

        v.resize(8,0xffff);
        BOOST_CHECK_EQUAL( v.size(), 8u );
        BOOST_CHECK_EQUAL( p->data().size(), 17u );
        BOOST_CHECK_EQUAL( v[6], 0xffffu );

#       undef v
    }
}

BOOST_AUTO_UNIT_TEST(parseVec_wrapper)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));
    typedef senf::Parse_Vector<
        senf::Parse_UInt16,
        senf::detail::Parse_VectorN_Sizer<senf::Parse_UInt8, 1u>
        > Parse_UInt16Vec;
    Parse_UInt16Vec v (boost::next(p->data().begin(),1), &p->data());
    Parse_UInt16Vec::container w (v);

    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[2], 0x1415 );
    BOOST_CHECK_EQUAL( w.size(), 3u );
    p->data()[0] = 0x06;
    BOOST_CHECK_EQUAL( w.size(), 6u );
    BOOST_CHECK_EQUAL( std::distance(w.begin(),w.end()), 
                       Parse_UInt16Vec::difference_type(w.size()) );

    w.shift(w.begin()+1);
    BOOST_CHECK_EQUAL( w.size(), 7u );
    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[1], 0 );
    BOOST_CHECK_EQUAL( w[2], 0x1213 );

    w.insert(w.begin()+3, 2u, 0xfffe);
    BOOST_CHECK_EQUAL( w.size(), 9u );
    BOOST_CHECK_EQUAL( w[2], 0x1213 );
    BOOST_CHECK_EQUAL( w[3], 0xfffe );
    BOOST_CHECK_EQUAL( w[4], 0xfffe );
    BOOST_CHECK_EQUAL( w[5], 0x1415 );

    w.erase(w.begin()+3, w.begin()+5);
    BOOST_CHECK_EQUAL( w.size(), 7u );

    w.erase(w.begin()+1);
    BOOST_CHECK_EQUAL( w.size(), 6u );

    {
        senf::PacketData::iterator i (p->data().begin()+1);
        Parse_UInt16Vec::iterator j (w.begin());
        Parse_UInt16Vec::iterator e (w.end());
        for (;j!=e;++j, i+=2)
            BOOST_CHECK_EQUAL( senf::Parse_UInt16(i,&p->data()), *j );
        BOOST_CHECK_EQUAL(p->data().end()-i, 0);
    }

    w.clear();
    BOOST_CHECK_EQUAL( w.size(), 0u );
    BOOST_CHECK( w.begin() == w.end() );
    BOOST_CHECK_EQUAL( p->data().size(), 1u );

    BOOST_CHECK_EQUAL( w.parser().size(), 0u );
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
