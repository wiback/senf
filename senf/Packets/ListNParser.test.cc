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
    \brief ListNParser unit tests */

//#include "ListNParser.test.hh"
//#include "ListNParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket_Type : public senf::PacketTypeBase
    {};
    typedef senf::ConcretePacket<VoidPacket_Type> VoidPacket;

    struct MyVec : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( size, senf::UInt8Parser );
        SENF_PARSER_VECTOR( vec, size, senf::UInt16Parser );

        SENF_PARSER_FINALIZE(MyVec);

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
        MyVec const & operator= (value_type const & other) { value(other); return *this; }
    };

    typedef senf::ListParser<
        senf::detail::ListNParser_Policy<
            MyVec,
            senf::detail::PrefixAuxParserPolicy<
                senf::UInt16Parser> > > MyListNParser;
}

SENF_AUTO_UNIT_TEST(ListNParser_container)
{
    VoidPacket vp (VoidPacket::create(MyListNParser::init_bytes));
    MyListNParser(vp.data().begin(),&vp.data()).init();

    {
        MyListNParser::container c (MyListNParser(vp.data().begin(),&vp.data()));

        c.push_back_space(2u);
        BOOST_CHECK_EQUAL( std::distance(c.begin(), c.end()), 2 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.begin().raw()), 2 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.front().vec().begin().raw()), 3 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.front().vec().end().raw()), 3 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.back().vec().begin().raw()), 4 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.back().vec().end().raw()), 4 );
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), c.end().raw()), 4 );
        BOOST_CHECK_EQUAL( c.bytes(), 4u );

        MyListNParser::container::iterator i (c.begin());
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), i->i()), 2 );
        BOOST_CHECK( i != c.end() );
        ++i;
        BOOST_CHECK_EQUAL( std::distance(c.data().begin(), i->i()), 3 );
        BOOST_CHECK( i != c.end() );
        ++i;
        BOOST_CHECK( i == c.end() );
    }
}

SENF_AUTO_UNIT_TEST(ListNParser)
{
    VoidPacket vp (VoidPacket::create(MyListNParser::init_bytes));

    {
        MyListNParser p (vp.data().begin(),&vp.data());
        p.init();
        BOOST_CHECK_EQUAL( p.size(), 0u );
        BOOST_CHECK_EQUAL( p.bytes(), 2u );
        BOOST_CHECK( p.empty() );
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
        BOOST_CHECK_EQUAL( p.front().vec().size(), 2u );
        BOOST_CHECK_EQUAL( p.bytes(), 7u );

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.size(), 2u );
        BOOST_CHECK_EQUAL( p.bytes(), 8u );
        BOOST_CHECK_EQUAL( p.back().vec().size(), 0u );

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

namespace {

    struct TestTransform
    {
        typedef unsigned value_type;
        static unsigned get(unsigned v) { return v/2; }
        static unsigned set(unsigned v) { return 2*v; }
    };

    struct TestListParserBase
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD_RO ( size1 , senf::UInt8Parser );
        SENF_PARSER_FIELD_RO ( size2 , senf::UInt8Parser );

        SENF_PARSER_FINALIZE(TestListParserBase);
    };

    struct TestListParser
        : public TestListParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_INHERIT ( TestListParserBase );

        SENF_PARSER_FIELD   ( dummy , senf::UInt32Parser );
        SENF_PARSER_LIST    ( list1  , transform(TestTransform, size1) , MyVec );
        SENF_PARSER_LIST    ( list2  , size2 , MyVec );

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

SENF_AUTO_UNIT_TEST(listMacro)
{
    unsigned char data[] = { 0x04,                   // size1
                             0x03,                   // size2
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

SENF_AUTO_UNIT_TEST(listMacro_stress)
{
    TestListPacket testListPacket (TestListPacket::create());
    for (unsigned i=0; i<42; ++i) {
        MyVec::value_type vec( 4, 42);
        testListPacket->list2().push_back( vec);
    }
    BOOST_CHECK_EQUAL( testListPacket->list2().size(), 42u );

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
