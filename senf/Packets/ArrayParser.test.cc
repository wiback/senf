//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ArrayParser unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    struct SomePacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        typedef senf::ArrayParser<2,senf::UInt24Parser> ArrayParser2;

        SENF_PARSER_FIELD( array, ArrayParser2 );
        SENF_PARSER_FIELD( index, senf::UInt16Parser );

        SENF_PARSER_FINALIZE(SomePacketParser);
    };

    struct SomeOtherParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        typedef senf::ArrayParser<1,SomePacketParser> ArrayParser1;

        SENF_PARSER_FIELD( fields, ArrayParser1 );

        SENF_PARSER_FINALIZE(SomeOtherParser);
    };
}

SENF_AUTO_TEST_CASE(ArrayParser_test)
{
    senf::PacketParserBase::byte data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));

    {
        typedef senf::ArrayParser<6,senf::UInt8Parser> UInt8ParserArray6;
        UInt8ParserArray6 v (p->data().begin(),&p->data());

        BOOST_CHECK_EQUAL( v[0], 0x00 );
        BOOST_CHECK_EQUAL( v[5], 0x05 );
        BOOST_CHECK_EQUAL( *v.begin(), 0x00 );
        BOOST_CHECK_EQUAL( std::distance(v.begin(),v.end()),
                           UInt8ParserArray6::difference_type(v.size()) );
        BOOST_CHECK_EQUAL( v.size(), 6u );
        UInt8ParserArray6::iterator i1 (v.begin());
        UInt8ParserArray6::iterator i2 (v.begin());
        ++i1;
        BOOST_CHECK_EQUAL( *i1, 0x01 );
        BOOST_CHECK_EQUAL( i1[-1], 0x00 );
        BOOST_CHECK_EQUAL( i1-i2, 1 );
        BOOST_CHECK_EQUAL( i2-i1, -1 );
        --i1;
        BOOST_CHECK( i1==i2 );
    }

    {
        SomeOtherParser v (p->data().begin(),&p->data());

        BOOST_CHECK_THROW( v.fields(), senf::TruncatedPacketException );
    }
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
