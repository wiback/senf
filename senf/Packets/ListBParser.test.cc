// $Id$
//
// Copyright (C) 2007
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
    \brief ListBParser unit tests */

//#include "ListBParser.test.hh"
//#include "ListBParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    struct VectorParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( size, senf::UInt8Parser );
        SENF_PARSER_VECTOR( vec, size, senf::UInt16Parser );

        SENF_PARSER_FINALIZE(VectorParser);

        typedef std::vector<boost::uint16_t> value_type;

        value_type value() const {
            value_type v (vec().begin(), vec().end());
            return v;
        }
        void value(value_type const & v) {
            vec_t::container container (vec());
            container.clear();
            for (value_type::const_iterator i=v.begin(); i!=v.end(); ++i)
                container.push_back( *i);
        }
        operator value_type() const { return value(); }
        VectorParser const & operator= (value_type const & other) { value(other); return *this; }
    };

    typedef senf::ListParser<
        senf::detail::ListBParser_Policy<
            VectorParser,
            senf::detail::PrefixAuxParserPolicy<senf::UInt16Parser> > > MyListBParser;
}

SENF_AUTO_UNIT_TEST(ListBParser)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            MyListBParser::init_bytes));

    MyListBParser p (pi->data().begin(),&pi->data());
    p.init();
    BOOST_CHECK_EQUAL( p.size(), 0u );
    BOOST_CHECK_EQUAL( p.bytes(), 2u );
    BOOST_CHECK( p.empty() );

    // the mutators are really tested together with the container wrappers since they are based
    // on the container wrapper. Here we only need one call to make the list larger ...

    p.push_back_space();
    p = MyListBParser(pi->data().begin(),&pi->data());
    BOOST_CHECK_EQUAL( p.bytes(), 3u );
    BOOST_CHECK_EQUAL( p.size(), 1u );
    BOOST_CHECK( ! p.empty() );
}

SENF_AUTO_UNIT_TEST(ListBParser_container)
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
            {
                VectorParser::vec_t::container c2v (c2.push_back_space().vec());
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

namespace {

    struct TestTransform
    {
        typedef unsigned value_type;
        static unsigned get(unsigned v) { return v/2; }
        static unsigned set(unsigned v) { return 2*v; }
    };

    struct TestListParser
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD ( size1 , senf::UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( size2 , senf::UInt8Parser );
        SENF_PARSER_FIELD         ( dummy , senf::UInt32Parser );
        SENF_PARSER_LIST          ( list1  , bytes(size1) , VectorParser );
        SENF_PARSER_LIST          ( list2  , transform(TestTransform, bytes(size2)) ,
                                             VectorParser );

        SENF_PARSER_FINALIZE(TestListParser);
    };

    struct TestListPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TestListPacketType>
    {
        typedef senf::PacketTypeMixin<TestListPacketType> mixin;
        typedef senf::ConcretePacket<TestListPacketType> packet;
        typedef TestListParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;
    };
    typedef senf::ConcretePacket<TestListPacketType> TestListPacket;

}

SENF_AUTO_UNIT_TEST(listBytesMacro)
{
    unsigned char data[] = {    8,                   // size1
                               18,                   // size2
                             0x01, 0x02, 0x03, 0x04, // dummy
                             0x01,                   // list1()[0].size()
                             0x05, 0x06,             // list1().vec()[0]
                             0x02,                   // list1()[1].size()
                             0x07, 0x08,             // list1()[1].vec()[0]
                             0x09, 0x0A,             // list1()[1].vec()[1]
                             0x00,                   // list2()[0].size()
                             0x02,                   // list2()[1].size()
                             0x0B, 0x0C,             // list2()[1].vec()[0]
                             0x0D, 0x0E,             // list2()[1].vec()[1]
                             0x01,                   // list2()[2].size()
                             0x0F, 0x10 };           // list2()[2].vec()[0]

    senf::DataPacket p (senf::DataPacket::create(data));
    TestListParser parser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL( parser.list1().size(), 2u );
    BOOST_CHECK_EQUAL( parser.list2().size(), 3u );
    BOOST_CHECK_EQUAL( parser.dummy(), 0x01020304u );

    TestListParser::list2_t::container list2 (parser.list2());

    {
        TestListParser::list1_t::container list (parser.list1());
        BOOST_CHECK_EQUAL( list.size(), 2u );

        TestListParser::list1_t::container::iterator i (list.begin());
        BOOST_CHECK_EQUAL( i->vec().size(), 1u );
        BOOST_CHECK_EQUAL( i->vec()[0], 0x0506u );

        ++i;
        BOOST_CHECK_EQUAL( i->vec().size(), 2u );
        BOOST_CHECK_EQUAL( i->vec()[0], 0x0708u );
        BOOST_CHECK_EQUAL( i->vec()[1], 0x090Au );

        ++i;
        BOOST_CHECK( i == list.end() );
    }

    {
        TestListParser::list2_t::container list (parser.list2());
        BOOST_CHECK_EQUAL( list.size(), 3u );

        TestListParser::list2_t::container::iterator i (list.begin());
        BOOST_CHECK_EQUAL( i->vec().size(), 0u );

        ++i;
        BOOST_CHECK_EQUAL( i->vec().size(), 2u );
        BOOST_CHECK_EQUAL( i->vec()[0], 0x0B0Cu );
        BOOST_CHECK_EQUAL( i->vec()[1], 0x0D0Eu );

        ++i;
        BOOST_CHECK_EQUAL( i->vec().size(), 1u );
        BOOST_CHECK_EQUAL( i->vec()[0], 0x0F10u );

        ++i;
        BOOST_CHECK( i == list.end() );
    }

}

namespace {

    struct TestPacketSizeList
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_LIST ( list, packetSize(), VectorParser );

        SENF_PARSER_FINALIZE(TestPacketSizeList);
    };
}

SENF_AUTO_UNIT_TEST(listBytesParser_packetSize)
{
    unsigned char data[] = { 0x01,                   // list()[0].vec().size()
                             0x05, 0x06,             // list()[0].vec()[0]
                             0x02,                   // list()[1].vec().size()
                             0x07, 0x08,             // list()[1].vec()[0]
                             0x09, 0x0A,             // list()[1].vec()[1]
                             0x00,                   // list()[2].vec().size()
                             0x02,                   // list()[3].vec().size()
                             0x0B, 0x0C,             // list()[3].vec()[0]
                             0x0D, 0x0E,             // list()[3].vec()[1]
                             0x01,                   // list()[4].vec().size()
                             0x0F, 0x10 };           // list()[4].vec()[0]

    senf::DataPacket p (senf::DataPacket::create(data));

    {
        TestPacketSizeList l (p.data().begin(), &p.data());
        BOOST_CHECK_EQUAL( l.list().size(), 5u );

        TestPacketSizeList::list_t::container c (l.list());
        TestPacketSizeList::list_t::container::iterator i (c.begin());

        senf::UInt16Parser::value_type vec0[] = { 0x0506 };
        senf::UInt16Parser::value_type vec1[] = { 0x0708, 0x090A };
        senf::UInt16Parser::value_type vec2[] = {};
        senf::UInt16Parser::value_type vec3[] = { 0x0B0C, 0x0D0E };
        senf::UInt16Parser::value_type vec4[] = { 0x0F10 };

        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec0, vec0+sizeof(vec0)/sizeof(vec0[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec1, vec1+sizeof(vec1)/sizeof(vec1[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec2, vec2+sizeof(vec2)/sizeof(vec2[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec3, vec3+sizeof(vec3)/sizeof(vec3[0]) );

        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec4, vec4+sizeof(vec4)/sizeof(vec4[0]) );

        ++ i;
        BOOST_CHECK( i == c.end() );

        i = c.begin();
        ++i;
        TestPacketSizeList::list_t::value_type::vec_t::container v (i->vec());
        v.push_back(0xFEFF);
    }

    {
        TestPacketSizeList l (p.data().begin(), &p.data());
        BOOST_CHECK_EQUAL( l.list().size(), 5u );
        BOOST_CHECK_EQUAL( l.list().bytes(), p.data().size() );

        TestPacketSizeList::list_t::container c (l.list());
        TestPacketSizeList::list_t::container::iterator i (c.begin());

        senf::UInt16Parser::value_type vec0[] = { 0x0506 };
        senf::UInt16Parser::value_type vec1[] = { 0x0708, 0x090A, 0xFEFF };
        senf::UInt16Parser::value_type vec2[] = {};
        senf::UInt16Parser::value_type vec3[] = { 0x0B0C, 0x0D0E };
        senf::UInt16Parser::value_type vec4[] = { 0x0F10 };

        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec0, vec0+sizeof(vec0)/sizeof(vec0[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec1, vec1+sizeof(vec1)/sizeof(vec1[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec2, vec2+sizeof(vec2)/sizeof(vec2[0]) );
        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec3, vec3+sizeof(vec3)/sizeof(vec3[0]) );

        ++ i;
        BOOST_CHECK_EQUAL_COLLECTIONS( i->vec().begin(), i->vec().end(),
                                       vec4, vec4+sizeof(vec4)/sizeof(vec4[0]) );
        ++ i;
        BOOST_CHECK( i == c.end() );
    }
}

SENF_AUTO_UNIT_TEST(listBytesMacro_stress)
{
    TestListPacket testListPacket (TestListPacket::create());
    for (unsigned i=0; i<12; ++i) {
        VectorParser::value_type vec( 4, 42);
        testListPacket->list2().push_back( vec);
    }
    BOOST_CHECK_EQUAL( testListPacket->list2().size(), 12u );

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
