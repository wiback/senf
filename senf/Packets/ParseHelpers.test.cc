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
    \brief ParseHelpers unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct FixedBaseParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD        ( normalField          , senf::UInt16Parser );
        SENF_PARSER_FIELD_RO     ( roField              , senf::UInt16Parser );

        SENF_PARSER_CUSTOM_FIELD ( customField          , int, 2             ) {
            return parse<senf::UInt16Parser>(customField_offset);
        }

        SENF_PARSER_BITFIELD     ( signedBitfield       , 4, signed          );
        SENF_PARSER_BITFIELD     ( unsignedBitfield     , 3, unsigned        );
        SENF_PARSER_BITFIELD     ( boolBitfield         , 1, bool            );

        SENF_PARSER_BITFIELD_RO  ( roSignedBitfield     , 4, signed          );
        SENF_PARSER_BITFIELD_RO  ( roUnsignedBitfield   , 3, unsigned        );
        SENF_PARSER_BITFIELD_RO  ( roBoolBitfield       , 1, bool            );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( roField );

        SENF_PARSER_FIELD        ( overlayOfRoField     , senf::Int16Parser  );
        SENF_PARSER_SKIP( 2 );
        SENF_PARSER_FIELD        ( overlayOfBitfield    , senf::UInt8Parser  );

        SENF_PARSER_GOTO_OFFSET( 1 );

        SENF_PARSER_PRIVATE_FIELD ( privLowbyteOfNormalField , senf::UInt8Parser  );

        unsigned lowbyteOfNormalField() {
            return privLowbyteOfNormalField();
        }

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( FixedBaseParser );
    };

    struct FixedDerivedParser : public FixedBaseParser
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_INHERIT( FixedBaseParser );

        SENF_PARSER_FIELD        ( derivedField         , senf::UInt16Parser );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( signedBitfield );

        SENF_PARSER_FIELD        ( anotherOverlay       , senf::UInt16Parser );

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( FixedDerivedParser )
    };
}

SENF_AUTO_TEST_CASE(fixedParser)
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

        SENF_PARSER_FIELD        ( normalField              , senf::UInt16Parser );
        SENF_PARSER_FIELD_RO     ( roField                  , senf::UInt16Parser );

        SENF_PARSER_CUSTOM_FIELD ( customField              , int, 2, 2          ) {
            return parse<senf::UInt16Parser>(customField_offset());
        }

        SENF_PARSER_BITFIELD      ( signedBitfield           , 4, signed          );
        SENF_PARSER_BITFIELD      ( unsignedBitfield         , 3, unsigned        );
        SENF_PARSER_BITFIELD      ( boolBitfield             , 1, bool            );

        SENF_PARSER_BITFIELD_RO   ( roSignedBitfield         , 4, signed          );
        SENF_PARSER_BITFIELD_RO   ( roUnsignedBitfield       , 3, unsigned        );
        SENF_PARSER_BITFIELD_RO   ( roBoolBitfield           , 1, bool            );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( roField );

        SENF_PARSER_FIELD         ( overlayOfRoField         , senf::Int16Parser  );
        SENF_PARSER_SKIP( 2, 2 );
        SENF_PARSER_FIELD         ( overlayOfBitfield        , senf::UInt8Parser  );

        SENF_PARSER_GOTO_OFFSET( 1, 1 );

        SENF_PARSER_PRIVATE_FIELD ( privLowbyteOfNormalField , senf::UInt8Parser  );

        unsigned lowbyteOfNormalField() {
            return privLowbyteOfNormalField();
        }

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( VariableBaseParser );
    };

    struct VariableDerivedParser : public VariableBaseParser
    {
#       include SENF_PARSER()

        SENF_PARSER_INHERIT( VariableBaseParser );

        SENF_PARSER_FIELD        ( derivedField         , senf::UInt16Parser );

        SENF_PARSER_LABEL( end );
        SENF_PARSER_GOTO( signedBitfield );

        SENF_PARSER_FIELD        ( anotherOverlay       , senf::UInt16Parser );

        SENF_PARSER_GOTO( end );

        SENF_PARSER_FINALIZE( VariableDerivedParser );
    };
}

SENF_AUTO_TEST_CASE(variableParser)
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
