// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief AuxParser unit tests */

//#include "AuxParser.test.hh"
//#include "AuxParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        UInt8VectorParser::container_type w (v);

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
        UInt16VectorParser::container_type w (v);

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
        ListParser::container_type w (l);

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
