// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief PacketParser.test unit tests */

//#include "PacketParser.test.hh"
//#include "PacketParser.test.ih"

// Custom includes
#include "PacketParser.hh"
#include "PacketInterpreter.hh"
#include "PacketType.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};

    struct SimpleParser : public senf::PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(SimpleParser);
        
        using senf::PacketParserBase::check;
        using senf::PacketParserBase::validate;
    };

    struct FooParser : public senf::PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(FooParser);

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field)( name, senf::Parse_UInt16 ))
            ((Field)( id,   senf::Parse_Int32  )) );
    };

    struct BarParser : public senf::PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(BarParser);

        SENF_PACKET_PARSER_DEFINE_FIELDS(
            ((Field)( name, senf::Parse_UInt16 ))
            ((Field)( id,   senf::Parse_Int32  )) );
    };
}

BOOST_AUTO_UNIT_TEST(packetParserBase)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(6u));
    SimpleParser p (pi->data().begin(),&pi->data());

    BOOST_CHECK( pi->data().begin() == p.i() );
    BOOST_CHECK( p.check(6u) );
    BOOST_CHECK( ! p.check(7u) );
    BOOST_CHECK_NO_THROW( p.validate(6u) );
    BOOST_CHECK_THROW( p.validate(7u), senf::TruncatedPacketException );

    // ?? Why the heck do I need the +0? I get an 'undefined symbol FooParser::fixed_bytes'
    // otherwise ...
    BOOST_CHECK_EQUAL( FooParser::fixed_bytes+0, 6u );
    BOOST_CHECK_EQUAL( BarParser(pi->data().begin(),&pi->data()).bytes(), 6u );
    BOOST_CHECK_EQUAL( senf::bytes(senf::Parse_UInt16(pi->data().begin(),&pi->data())), 2u );
    BOOST_CHECK_EQUAL( senf::bytes(FooParser(pi->data().begin(),&pi->data())), 6u );
    BOOST_CHECK_EQUAL( senf::bytes(BarParser(pi->data().begin(),&pi->data())), 6u );

    BOOST_CHECK_EQUAL( senf::init_bytes<FooParser>::value, 6u );
    BOOST_CHECK_EQUAL( senf::init_bytes<BarParser>::value, 6u );
}

BOOST_AUTO_UNIT_TEST(safePacketParser)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(6u));
    senf::SafePacketParser<senf::Parse_UInt16> p;
    
    BOOST_CHECK( !p );

    p =  senf::Parse_UInt16(pi->data().begin(),&pi->data());

    BOOST_CHECK( p );
    (*p) = 0x1234u;
    
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    BOOST_CHECK_EQUAL( p->data()[0], 0x12u );

    p->data().resize(1024u);
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    (*p) = 0x2345u;
    BOOST_CHECK_EQUAL( p->data()[0], 0x23u );
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
