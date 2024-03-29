//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief VectorParser unit tests */

#include "Packets.hh"

// Custom includes
#include <boost/assign.hpp>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};
}

SENF_AUTO_TEST_CASE(VectorParser)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));
    typedef senf::VectorParser<
        senf::UInt16Parser,
        senf::detail::FixedAuxParserPolicy<senf::UInt8Parser, 1u>
        > UInt16VectorParser;

    {
        UInt16VectorParser v (boost::next(p->data().begin(), 1), &p->data());

        BOOST_CHECK_EQUAL( v[0], 0x1011 );
        BOOST_CHECK_EQUAL( v[2], 0x1415 );
        BOOST_CHECK_EQUAL( v.size(), 3u );
        BOOST_CHECK_EQUAL( v.bytes(), 6u );
        BOOST_CHECK( ! v.empty() );
        p->data()[0] = 0x06;
        BOOST_CHECK_EQUAL( v.size(), 6u );
        BOOST_CHECK_EQUAL( v.bytes(), 12u );

        UInt16VectorParser::iterator b (v.begin());
        UInt16VectorParser::iterator e (v.end());
        BOOST_CHECK_EQUAL(std::distance(b,e), UInt16VectorParser::difference_type(v.size()));
    }

    // Warning: Each of the following operations invalidate the parser -> we need to recreate it at
    // each step

    // And since all these members directly call the corresponding members in the container wrapper,
    // we don't need to check them again below ...

    {
#       define v UInt16VectorParser(boost::next(p->data().begin(),1),&p->data())

        v.push_back(0xf0f1u,2);
        BOOST_CHECK_EQUAL( v.size(), 8u );
        BOOST_CHECK_EQUAL( v[7], 0xf0f1u );

        v.push_back_space() = 37u;
        BOOST_CHECK_EQUAL( v.size(), 9u );
        BOOST_CHECK_EQUAL( v[8], 37u );

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

SENF_AUTO_TEST_CASE(VectorParser_wrapper)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));
    typedef senf::VectorParser<
        senf::UInt16Parser,
        senf::detail::FixedAuxParserPolicy<senf::UInt8Parser, 1u>
        > UInt16VectorParser;
    UInt16VectorParser v (boost::next(p->data().begin(),1), &p->data());
    UInt16VectorParser::container_type w (v);

    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[2], 0x1415 );
    BOOST_CHECK_EQUAL( w.size(), 3u );
    p->data()[0] = 0x06;
    BOOST_CHECK_EQUAL( w.size(), 6u );
    BOOST_CHECK_EQUAL( std::distance(w.begin(),w.end()),
                       UInt16VectorParser::difference_type(w.size()) );

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
        UInt16VectorParser::iterator j (w.begin());
        UInt16VectorParser::iterator e (w.end());
        for (;j!=e;++j, i+=2)
            BOOST_CHECK_EQUAL( senf::UInt16Parser(i,&p->data()), *j );
        BOOST_CHECK_EQUAL(p->data().end()-i, 0);
    }

    w.clear();
    BOOST_CHECK_EQUAL( w.size(), 0u );
    BOOST_CHECK( w.begin() == w.end() );
    BOOST_CHECK_EQUAL( p->data().size(), 1u );

    BOOST_CHECK_EQUAL( w.parser().size(), 0u );
}

SENF_AUTO_TEST_CASE(dynamicPolicyVector)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));

    typedef senf::VectorParser<
        senf::UInt16Parser,
        senf::detail::DynamicAuxParserPolicy<senf::UInt8Parser>
        > UInt16VectorParser;

    UInt16VectorParser v (senf::UInt8Parser(p->data().begin(), &p->data()),
                          boost::next(p->data().begin(),1), &p->data());
    UInt16VectorParser::container_type w (v);

    BOOST_CHECK_EQUAL( v.size(), 3u );
    BOOST_CHECK_EQUAL( w.size(), 3u );

    BOOST_CHECK_EQUAL( v[0], 0x1011 );
    BOOST_CHECK_EQUAL( v[2], 0x1415 );

    BOOST_CHECK_EQUAL( w[0], 0x1011 );
    BOOST_CHECK_EQUAL( w[2], 0x1415 );
}

namespace {

    struct TestTransform
    {
        typedef unsigned value_type;
        static unsigned get(unsigned v) { return v-2; }
        static unsigned set(unsigned v) { return v+2; }
    };

    struct TestVectorParser
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD ( size1 , senf::UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( size2 , senf::UInt8Parser );
        SENF_PARSER_FIELD         ( dummy , senf::UInt32Parser );
        SENF_PARSER_VECTOR        ( vec1  , transform(TestTransform, size1) , senf::UInt16Parser );
        SENF_PARSER_VECTOR        ( vec2  , bytes(size2) , senf::UInt16Parser );

        SENF_PARSER_FINALIZE( TestVectorParser );
    };

    struct TestVectorPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TestVectorPacketType>
    {
        typedef senf::PacketTypeMixin<TestVectorPacketType> mixin;
        typedef TestVectorParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef senf::ConcretePacket<TestVectorPacketType> TestVectorPacket;

}

SENF_AUTO_TEST_CASE(vectorMacro_parse)
{
    unsigned char data[] = { 0x05,                   // size1
                             0x04,                   // size2
                             0x01, 0x02, 0x03, 0x04, // dummy
                             0x05, 0x06,             // vec1[0]
                             0x07, 0x08,             // vec1[1]
                             0x09, 0x0A,             // vec1[2]
                             0x0B, 0x0C,             // vec2[0]
                             0x0D, 0x0E };           // vec2[1]

    senf::DataPacket p (senf::DataPacket::create(data));
    TestVectorParser parser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL( parser.vec1().size(), 3u );
    BOOST_CHECK_EQUAL( parser.vec2().size(), 2u );
    BOOST_CHECK_EQUAL( parser.dummy(), 0x01020304u );
    BOOST_CHECK_EQUAL( parser.vec1()[0], 0x0506u );
    BOOST_CHECK_EQUAL( parser.vec1()[1], 0x0708u );
    BOOST_CHECK_EQUAL( parser.vec1()[2], 0x090Au );
    BOOST_CHECK_EQUAL( parser.vec2()[0], 0x0B0Cu );
    BOOST_CHECK_EQUAL( parser.vec2()[1], 0x0D0Eu );

}

SENF_AUTO_TEST_CASE(vectorMacro_create)
{
    TestVectorPacket p (TestVectorPacket::create());
    p->dummy() = 0x01020304u;
    p->vec1().push_back( 0x0506u);
    p->vec1().push_back( 0x0708u);
    p->vec1().push_back( 0x090Au);
    p->vec2().push_back( 0x0B0Cu);
    p->vec2().push_back( 0x0D0Eu);
    p.finalizeAll();

    unsigned char data[] = {
            0x05,                   // size1
            0x04,                   // size2
            0x01, 0x02, 0x03, 0x04, // dummy
            0x05, 0x06,             // vec1[0]
            0x07, 0x08,             // vec1[1]
            0x09, 0x0A,             // vec1[2]
            0x0B, 0x0C,             // vec2[0]
            0x0D, 0x0E              // vec2[1]
    };
    SENF_CHECK_EQUAL_COLLECTIONS(
            data, data+sizeof(data), p.data().begin(), p.data().end() );
}

namespace {

    struct TestVectorBaseParser
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD ( size1 , senf::UInt8Parser );
        SENF_PARSER_FIELD_RO      ( size2 , senf::UInt8Parser );
        SENF_PARSER_FIELD         ( dummy , senf::UInt32Parser );\

        SENF_PARSER_FINALIZE( TestVectorBaseParser );
    };

    struct TestVectorDerivedParser
        : public TestVectorBaseParser
    {
#       include SENF_PARSER()

        SENF_PARSER_INHERIT(TestVectorBaseParser);

        SENF_PARSER_VECTOR        ( vec1  , transform(TestTransform, size1) , senf::UInt8Parser );
        SENF_PARSER_VECTOR        ( vec2  , bytes(size2) , senf::UInt16Parser );

        SENF_PARSER_FINALIZE( TestVectorDerivedParser );
    };

}

SENF_AUTO_TEST_CASE(vectorMacro_inherit)
{
    unsigned char data[] = { 0x08,                   // size1
                             0x04,                   // size2
                             0x01, 0x02, 0x03, 0x04, // dummy
                             0x05, 0x06,             // vec1[0], vec1[1]
                             0x07, 0x08,             // vec1[2], vec1[3]
                             0x09, 0x0A,             // vec1[4], vec1[5]
                             0x0B, 0x0C,             // vec2[0]
                             0x0D, 0x0E };           // vec2[1]

    senf::DataPacket p (senf::DataPacket::create(data));
    TestVectorDerivedParser parser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL( parser.vec1().size(), 6u );
    BOOST_CHECK_EQUAL( parser.vec2().size(), 2u );
    BOOST_CHECK_EQUAL( parser.dummy(), 0x01020304u );
    BOOST_CHECK_EQUAL( parser.vec2()[0], 0x0B0Cu );
    BOOST_CHECK_EQUAL( parser.vec2()[1], 0x0D0Eu );
    boost::uint8_t c = 0x05;
    for(auto const & v: parser.vec1()) {
        BOOST_CHECK_EQUAL( v, c++ );
    }
}

namespace {

    struct TestNonValueFixedSizeParser
        : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD ( value, senf::UInt8Parser );
        SENF_PARSER_FINALIZE( TestNonValueFixedSizeParser );
    };

    struct TestPacketSizeVectorParser
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_FIELD_RO ( size1, senf::UInt8Parser                 );
        SENF_PARSER_VECTOR   ( vec1, size1, TestNonValueFixedSizeParser );
        SENF_PARSER_VECTOR   ( vec2, packetSize(), senf::UInt16Parser   );
        SENF_PARSER_FINALIZE ( TestPacketSizeVectorParser               );
    };

}

SENF_AUTO_TEST_CASE(vectorMacro_packetSize)
{
    unsigned char data[] = {
            0x05, 0xaa, 0xbb, 0xcc, 0xdd, 0xee,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
            0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };

    senf::DataPacket p (senf::DataPacket::create(data));
    TestPacketSizeVectorParser parser (p.data().begin(), &p.data());

    {
        BOOST_REQUIRE_EQUAL( parser.size1(), 5u );
        BOOST_REQUIRE_EQUAL( parser.vec1().size(), 5u );
        BOOST_REQUIRE_EQUAL( parser.vec2().size(), 6u );
        BOOST_CHECK_EQUAL( parser.vec2()[0], 0x1112u );
        BOOST_CHECK_EQUAL( parser.vec2()[1], 0x1314u );
        BOOST_CHECK_EQUAL( parser.vec2()[5], 0x2526u );
    } {
        std::vector<boost::uint16_t> vec;
        std::copy( parser.vec2().begin(), parser.vec2().end(), std::inserter(vec, vec.begin()));
        BOOST_CHECK_EQUAL( vec.size(), 6u );
        BOOST_CHECK_EQUAL( vec[0], 0x1112u );
        BOOST_CHECK_EQUAL( vec[1], 0x1314u );
        BOOST_CHECK_EQUAL( vec[5], 0x2526u );

        TestPacketSizeVectorParser::vec2_t::container_type(parser.vec2()).clear();
        BOOST_CHECK_EQUAL( parser.vec2().size(), 0u );

        std::copy( vec.begin(), vec.end(), senf::back_inserter(parser.vec2()));
        BOOST_REQUIRE_EQUAL( parser.vec2().size(), 6u );
        BOOST_CHECK_EQUAL( parser.vec2()[0], 0x1112u );
        BOOST_CHECK_EQUAL( parser.vec2()[1], 0x1314u );
        BOOST_CHECK_EQUAL( parser.vec2()[5], 0x2526u );
    } {
        senf::DataPacket p2 (senf::DataPacket::create(TestPacketSizeVectorParser::init_bytes));
        TestPacketSizeVectorParser parser2 (p2.data().begin(), &p2.data());

        std::copy( parser.vec2().begin(), parser.vec2().end(), senf::back_inserter(parser2.vec2()));
        BOOST_REQUIRE_EQUAL( parser2.vec2().size(), 6u );
        BOOST_CHECK_EQUAL( parser2.vec2()[0], 0x1112u );
        BOOST_CHECK_EQUAL( parser2.vec2()[1], 0x1314u );
        BOOST_CHECK_EQUAL( parser2.vec2()[5], 0x2526u );
    } {
        senf::DataPacket p2 (senf::DataPacket::create(TestPacketSizeVectorParser::init_bytes));
        TestPacketSizeVectorParser parser2 (p2.data().begin(), &p2.data());

        std::copy( parser.vec1().begin(), parser.vec1().end(), senf::back_inserter(parser2.vec1()));
        BOOST_REQUIRE_EQUAL( parser2.vec1().size(), parser.size1() );
        BOOST_CHECK_EQUAL( parser2.vec1()[0].value(), 0xaa );
        BOOST_CHECK_EQUAL( parser2.vec1()[1].value(), 0xbb );
        BOOST_CHECK_EQUAL( parser2.vec1()[4].value(), 0xee );
    }

    // The real functionality is already tested in AuxPolixy.test.cc ...
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
