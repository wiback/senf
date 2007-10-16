// $Id$
//
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
    \brief ParseHelpers.test unit tests */

//#include "ParseHelpers.test.hh"
//#include "ParseHelpers.test.ih"

// Custom includes
#include "Packets.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct FixedBaseParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSE_FIELD        ( normalField          , senf::Parse_UInt16 );
        SENF_PARSE_FIELD_RO     ( roField              , senf::Parse_UInt16 );

        SENF_PARSE_CUSTOM_FIELD ( customField          , int, 2             ) {
            return parse<senf::Parse_UInt16>(customField_offset);
        }
        
        SENF_PARSE_BITFIELD     ( signedBitfield       , 4, signed          );
        SENF_PARSE_BITFIELD     ( unsignedBitfield     , 3, unsigned        );
        SENF_PARSE_BITFIELD     ( boolBitfield         , 1, bool            );

        SENF_PARSE_BITFIELD_RO  ( roSignedBitfield     , 4, signed          );
        SENF_PARSE_BITFIELD_RO  ( roUnsignedBitfield   , 3, unsigned        );
        SENF_PARSE_BITFIELD_RO  ( roBoolBitfield       , 1, bool            );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( roField );

        SENF_PARSE_FIELD        ( overlayOfRoField     , senf::Parse_Int16  );
        SENF_PARSER_SKIP( 2 );
        SENF_PARSE_FIELD        ( overlayOfBitfield    , senf::Parse_UInt8  );

        SENF_PARSER_GOTO_OFFSET( 1 );

        SENF_PARSE_FIELD        ( lowbyteOfNormalField , senf::Parse_UInt8  );

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( FixedBaseParser );
    };

    struct FixedDerivedParser : public FixedBaseParser
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_INHERIT( FixedBaseParser );
        
        SENF_PARSE_FIELD        ( derivedField         , senf::Parse_UInt16 );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( signedBitfield );

        SENF_PARSE_FIELD        ( anotherOverlay       , senf::Parse_UInt16 );
        
        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( FixedDerivedParser )
    };
}

BOOST_AUTO_UNIT_TEST(fixedParser)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x83, 0x84, 0x07, 0x08 };

    senf::DataPacket p (senf::DataPacket::create(data));

    FixedBaseParser baseParser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL( FixedBaseParser::fixed_bytes+0, 8u );

    BOOST_CHECK_EQUAL (   baseParser.normalField()          , 0x0102u );
    BOOST_CHECK_EQUAL (   baseParser.roField()              , 0x0304u );
    BOOST_CHECK_EQUAL (   baseParser.customField()          , 0x0506  );

    BOOST_CHECK_EQUAL (   baseParser.signedBitfield()       , -8      );
    BOOST_CHECK_EQUAL (   baseParser.unsignedBitfield()     , 1u      );
    BOOST_CHECK       (   baseParser.boolBitfield()                   );

    BOOST_CHECK_EQUAL (   baseParser.roSignedBitfield()     , -8      );
    BOOST_CHECK_EQUAL (   baseParser.roUnsignedBitfield()   , 2u      );
    BOOST_CHECK       ( ! baseParser.roBoolBitfield()                 );

    BOOST_CHECK_EQUAL (   baseParser.overlayOfRoField()     , 0x0304  );
    BOOST_CHECK_EQUAL (   baseParser.overlayOfBitfield()    , 0x83u   );
    BOOST_CHECK_EQUAL (   baseParser.lowbyteOfNormalField() , 0x02u   );

    FixedDerivedParser derivedParser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL( FixedDerivedParser::fixed_bytes+0, 10u );

    BOOST_CHECK_EQUAL (   derivedParser.normalField()          , 0x0102u );
    BOOST_CHECK_EQUAL (   derivedParser.roField()              , 0x0304u );
    BOOST_CHECK_EQUAL (   derivedParser.customField()          , 0x0506  );

    BOOST_CHECK_EQUAL (   derivedParser.signedBitfield()       , -8      );
    BOOST_CHECK_EQUAL (   derivedParser.unsignedBitfield()     , 1u      );
    BOOST_CHECK       (   derivedParser.boolBitfield()                   );

    BOOST_CHECK_EQUAL (   derivedParser.roSignedBitfield()     , -8      );
    BOOST_CHECK_EQUAL (   derivedParser.roUnsignedBitfield()   , 2u      );
    BOOST_CHECK       ( ! derivedParser.roBoolBitfield()                 );

    BOOST_CHECK_EQUAL (   derivedParser.overlayOfRoField()     , 0x0304  );
    BOOST_CHECK_EQUAL (   derivedParser.overlayOfBitfield()    , 0x83u   );
    BOOST_CHECK_EQUAL (   derivedParser.lowbyteOfNormalField() , 0x02u   );
    
    BOOST_CHECK_EQUAL (   derivedParser.derivedField()         , 0x0708u );
    BOOST_CHECK_EQUAL (   derivedParser.anotherOverlay()       , 0x8384u );
}

namespace {

    struct VariableBaseParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSE_FIELD        ( normalField          , senf::Parse_UInt16 );
        SENF_PARSE_FIELD_RO     ( roField              , senf::Parse_UInt16 );

        SENF_PARSE_CUSTOM_FIELD ( customField          , int, 2, 2          ) {
            return parse<senf::Parse_UInt16>(customField_offset());
        }
        
        SENF_PARSE_BITFIELD     ( signedBitfield       , 4, signed          );
        SENF_PARSE_BITFIELD     ( unsignedBitfield     , 3, unsigned        );
        SENF_PARSE_BITFIELD     ( boolBitfield         , 1, bool            );

        SENF_PARSE_BITFIELD_RO  ( roSignedBitfield     , 4, signed          );
        SENF_PARSE_BITFIELD_RO  ( roUnsignedBitfield   , 3, unsigned        );
        SENF_PARSE_BITFIELD_RO  ( roBoolBitfield       , 1, bool            );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( roField );

        SENF_PARSE_FIELD        ( overlayOfRoField     , senf::Parse_Int16  );
        SENF_PARSER_SKIP( 2, 2 );
        SENF_PARSE_FIELD        ( overlayOfBitfield    , senf::Parse_UInt8  );

        SENF_PARSER_GOTO_OFFSET( 1, 1 );

        SENF_PARSE_FIELD        ( lowbyteOfNormalField , senf::Parse_UInt8  );

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( VariableBaseParser );
    };

    struct VariableDerivedParser : public VariableBaseParser
    {
#       include SENF_PARSER()

        SENF_PARSER_INHERIT( VariableBaseParser );
        
        SENF_PARSE_FIELD        ( derivedField         , senf::Parse_UInt16 );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( signedBitfield );

        SENF_PARSE_FIELD        ( anotherOverlay       , senf::Parse_UInt16 );
        
        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( VariableDerivedParser );
    };
}

BOOST_AUTO_UNIT_TEST(variableParser)
{
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x83, 0x84, 0x07, 0x08 };

    senf::DataPacket p (senf::DataPacket::create(data));

    VariableBaseParser baseParser (p.data().begin(), &p.data());
    
    BOOST_CHECK_EQUAL ( senf::bytes(baseParser), 8u );

    BOOST_CHECK_EQUAL (   baseParser.normalField()          , 0x0102u );
    BOOST_CHECK_EQUAL (   baseParser.roField()              , 0x0304u );
    BOOST_CHECK_EQUAL (   baseParser.customField()          , 0x0506  );

    BOOST_CHECK_EQUAL (   baseParser.signedBitfield()       , -8      );
    BOOST_CHECK_EQUAL (   baseParser.unsignedBitfield()     , 1u      );
    BOOST_CHECK       (   baseParser.boolBitfield()                   );

    BOOST_CHECK_EQUAL (   baseParser.roSignedBitfield()     , -8      );
    BOOST_CHECK_EQUAL (   baseParser.roUnsignedBitfield()   , 2u      );
    BOOST_CHECK       ( ! baseParser.roBoolBitfield()                 );

    BOOST_CHECK_EQUAL (   baseParser.overlayOfRoField()     , 0x0304  );
    BOOST_CHECK_EQUAL (   baseParser.overlayOfBitfield()    , 0x83u   );
    BOOST_CHECK_EQUAL (   baseParser.lowbyteOfNormalField() , 0x02u   );

    VariableDerivedParser derivedParser (p.data().begin(), &p.data());

    BOOST_CHECK_EQUAL ( senf::bytes(derivedParser), 10u );

    BOOST_CHECK_EQUAL (   derivedParser.normalField()          , 0x0102u );
    BOOST_CHECK_EQUAL (   derivedParser.roField()              , 0x0304u );
    BOOST_CHECK_EQUAL (   derivedParser.customField()          , 0x0506  );

    BOOST_CHECK_EQUAL (   derivedParser.signedBitfield()       , -8      );
    BOOST_CHECK_EQUAL (   derivedParser.unsignedBitfield()     , 1u      );
    BOOST_CHECK       (   derivedParser.boolBitfield()                   );

    BOOST_CHECK_EQUAL (   derivedParser.roSignedBitfield()     , -8      );
    BOOST_CHECK_EQUAL (   derivedParser.roUnsignedBitfield()   , 2u      );
    BOOST_CHECK       ( ! derivedParser.roBoolBitfield()                 );

    BOOST_CHECK_EQUAL (   derivedParser.overlayOfRoField()     , 0x0304  );
    BOOST_CHECK_EQUAL (   derivedParser.overlayOfBitfield()    , 0x83u   );
    BOOST_CHECK_EQUAL (   derivedParser.lowbyteOfNormalField() , 0x02u   );
    
    BOOST_CHECK_EQUAL (   derivedParser.derivedField()         , 0x0708u );
    BOOST_CHECK_EQUAL (   derivedParser.anotherOverlay()       , 0x8384u );
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
