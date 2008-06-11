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
    \brief VariantParser.test unit tests */

//#include "VariantParser.test.hh"
//#include "VariantParser.test.ih"

// Custom includes
#include "Packets.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(VariantParser)
{
    typedef senf::ArrayParser<10, senf::UInt8Parser> Array10;
    typedef senf::VariantParser< senf::detail::FixedAuxParserPolicy<senf::UInt8Parser, 1>,
        boost::mpl::vector<senf::VoidPacketParser, Array10, senf:: UInt32Parser> > Variant;
    
    unsigned char data[] = { 0x01, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                             0x19, 0x1A, 0x1B };
    senf::DataPacket p (senf::DataPacket::create(data));
    {
        Variant v ( boost::next(p.data().begin()), & p.data() );
        BOOST_REQUIRE_EQUAL( v.variant(), 1u );
        BOOST_CHECK_EQUAL( senf::bytes(v), 10u );
        BOOST_CHECK_EQUAL_COLLECTIONS( v.get<1>().begin(), v.get<1>().end(),
                                       data+1, data+11 );
        v.init();
        // The container size should not change in this case but now variant should be 0 ...
        BOOST_REQUIRE_EQUAL( p.data().size(), 11u );
        BOOST_REQUIRE_EQUAL( v.variant(), 0u );
        BOOST_CHECK_EQUAL( senf::bytes(v), 0u );
        
        v.init<2>();
        // v invalidated
    }
    {
        Variant v ( boost::next(p.data().begin()), & p.data() );

        BOOST_CHECK_EQUAL( p.data()[0], 2u );
        BOOST_REQUIRE_EQUAL( v.variant(), 2u );
        BOOST_CHECK_EQUAL( senf::bytes(v), 4u );

        unsigned char data2[] = { 0x02, 0x00, 0x00, 0x00, 0x00,
                                  0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                  0x19, 0x1A, 0x1B };
        BOOST_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
                                       data2, data2+sizeof(data2) );
        BOOST_CHECK_EQUAL( v.get<2>().value(), 0u );
        BOOST_CHECK_EQUAL( p.data().size(), 15u );
        p.data()[4] = 0x01u;
        BOOST_CHECK_EQUAL( v.get<2>().value(), 1u );
    };
}

namespace {
    
    struct SubParser : public senf::PacketParserBase
    { 
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( foo, senf::UInt32Parser );
        SENF_PARSER_FIELD( bar, senf::Int16Parser );

        SENF_PARSER_FINALIZE(SubParser);
    };

    struct TestParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_SKIP_BITS( 4 );
        SENF_PARSER_BITFIELD_RO( type, 4, unsigned );
        SENF_PARSER_PRIVATE_VARIANT( content_, type,
                                         ( novalue( nocontent, key(10, senf::VoidPacketParser)) )
                                         (      id( content,           SubParser              ) )
            );

        SENF_PARSER_FINALIZE(TestParser);
    };
    
}

BOOST_AUTO_UNIT_TEST(VariantParserMacro)
{
    senf::DataPacket p (senf::DataPacket::create(senf::init_bytes<TestParser>::value));
    
    {
        TestParser v (p.data().begin(), & p.data());
        v.init();
        BOOST_CHECK( ! v.has_content() );
        BOOST_CHECK_EQUAL( senf::bytes(v), 1u );
        BOOST_CHECK_EQUAL( v.type(), 10u );
        v.init_content();
        // Parser invalidated
    }
    {
        TestParser v (p.data().begin(), & p.data());
        BOOST_CHECK( v.has_content() );
        BOOST_CHECK_EQUAL( senf::bytes(v), 7u );
        BOOST_CHECK_EQUAL( v.content().foo(), 0u );
        BOOST_CHECK_EQUAL( v.type(), 1u );
        v.nocontent();
        // Parser invalidated
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
