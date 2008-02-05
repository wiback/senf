// $Id$
//
// Copyright (C) 2006
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

// Unit tests

//#include "ArrayParser.test.hh"
//#include "ArrayParser.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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

BOOST_AUTO_UNIT_TEST(ArrayParser_test)
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
