// $Id$
//
// Copyright (C) 2009 
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
    \brief AuxParser unit tests */

//#include "AuxParser.test.hh"
//#include "AuxParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

// The other aux policies are tested with the corresponding container classes

SENF_AUTO_UNIT_TEST(vectorPacketSizeAuxPolicy)
{
    unsigned char data[] = { 0x10, 0x11,  0x12, 0x13,  0x14, 0x15,
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::Packet p (senf::DataPacket::create(data));

    typedef senf::VectorParser<
        senf::UInt8Parser,
        senf::detail::PacketSizeAuxParserPolicy
        > UInt8VectorParser;

    {
        UInt8VectorParser v (p.data().begin(), &p.data());

        BOOST_CHECK_EQUAL( v.size(), p.data().size() );
        BOOST_CHECK_EQUAL_COLLECTIONS( v.begin(), v.end(), data, data+sizeof(data) );
    }

    {
        UInt8VectorParser v (p.data().begin(), &p.data());
        UInt8VectorParser::container w (v);

        BOOST_CHECK_EQUAL_COLLECTIONS( w.begin(), w.end(), data, data+sizeof(data) );

        w.shift(w.begin()+1);
        BOOST_CHECK_EQUAL( w.size(), p.data().size() );

        w.erase(w.begin()+3, w.begin()+5);
        BOOST_CHECK_EQUAL( w.size(), p.data().size() );

        unsigned char data2[] = { 0x10, 0x00, 0x11, 0x14, 0x15,
                                  0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };

        BOOST_CHECK_EQUAL_COLLECTIONS( w.begin(), w.end(), data2, data2+sizeof(data2) );
    }
}

SENF_AUTO_UNIT_TEST(vectorPacketSizeAuxPolicy_transformed)
{
    unsigned char data[] = { 0x10, 0x11,  0x12, 0x13,  0x14, 0x15,
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::Packet p (senf::DataPacket::create(data));

    typedef senf::VectorParser<
        senf::UInt16Parser,
        senf::detail::TransformAuxParserPolicy<
            senf::detail::PacketSizeAuxParserPolicy,
            senf::detail::VectorParserBytesTransform<senf::UInt16Parser::fixed_bytes> >
        > UInt16VectorParser;

    {
        UInt16VectorParser v (p.data().begin(), &p.data());
        
        BOOST_REQUIRE_EQUAL( v.size(), p.data().size()/2 );
        BOOST_CHECK_EQUAL( v[0], 0x1011u );
        BOOST_CHECK_EQUAL( v[1], 0x1213u );
        BOOST_CHECK_EQUAL( v[5], 0x2425u );
    }

    {
        UInt16VectorParser v (p.data().begin(), &p.data());
        UInt16VectorParser::container w (v);

        BOOST_REQUIRE_EQUAL( w.size(), p.data().size()/2 );
        BOOST_CHECK_EQUAL( w[0], 0x1011u );
        BOOST_CHECK_EQUAL( w[1], 0x1213u );
        BOOST_CHECK_EQUAL( w[5], 0x2425u );

        w.shift(w.begin()+1);
        BOOST_CHECK_EQUAL( w.size(), p.data().size()/2 );

        w.erase(w.begin()+3, w.begin()+5);
        BOOST_CHECK_EQUAL( w.size(), p.data().size()/2 );

        senf::UInt16Parser::value_type  data2[] = 
            { 0x1011u, 0x0000u, 0x1213u, 0x2223u, 0x2425u };

        BOOST_CHECK_EQUAL_COLLECTIONS( w.begin(), w.end(), 
                                       data2, data2+sizeof(data2)/sizeof(data2[0]) );
    }
}

SENF_AUTO_UNIT_TEST(listPacketSizeAuxPolicy)
{
    unsigned char data[] = { 0x10, 0x11,  0x12, 0x13,  0x14, 0x15,
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::Packet p (senf::DataPacket::create(data));

    typedef senf::ListParser<
        senf::detail::ListBParser_Policy<
            senf::UInt16Parser,
            senf::detail::PacketSizeAuxParserPolicy>
        > ListParser;

    {
        ListParser l (p.data().begin(), &p.data());

        BOOST_REQUIRE_EQUAL( l.size(), p.data().size()/2 );
        BOOST_CHECK_EQUAL( l.front(), 0x1011u );
        BOOST_CHECK_EQUAL( l.back(), 0x2425u );
    }

    {
        ListParser l (p.data().begin(), &p.data());
        ListParser::container w (l);

        BOOST_REQUIRE_EQUAL( w.size(), p.data().size()/2 );
        BOOST_CHECK_EQUAL( *w.begin(), 0x1011u );
        BOOST_CHECK_EQUAL( *boost::next(w.begin()), 0x1213u );
        BOOST_CHECK_EQUAL( *boost::next(w.begin(),5), 0x2425u );

        w.shift(boost::next(w.begin()));
        BOOST_CHECK_EQUAL( w.size(), p.data().size()/2 );

        w.erase(boost::next(w.begin(),3), boost::next(w.begin(),5));
        BOOST_CHECK_EQUAL( w.size(), p.data().size()/2 );

        senf::UInt16Parser::value_type  data2[] = 
            { 0x1011u, 0x0000u, 0x1213u, 0x2223u, 0x2425u };

        BOOST_CHECK_EQUAL_COLLECTIONS( w.begin(), w.end(), 
                                       data2, data2+sizeof(data2)/sizeof(data2[0]) );
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
