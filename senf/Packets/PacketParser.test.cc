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
    \brief PacketParser unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};

    struct SimpleParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FINALIZE(SimpleParser);

        using senf::PacketParserBase::check;
        using senf::PacketParserBase::validate;
    };

    struct FooParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( name, senf::UInt16Parser );
        SENF_PARSER_FIELD( id,   senf::Int32Parser  );

        SENF_PARSER_FINALIZE(FooParser);
    };

    struct BarParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD( name, senf::UInt16Parser );
        SENF_PARSER_FIELD( id,   senf::Int32Parser  );

        SENF_PARSER_FINALIZE(BarParser);
    };

    unsigned UniqueNumber() {
        static unsigned current = 0;
        return ++current;
    }

    bool IsUniqueNumber(senf::PacketData::byte b) {
        static unsigned current = 0;
        return b != ++current;
    }
}

SENF_AUTO_TEST_CASE(packetParserBase)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(6u)));
    SimpleParser p (pi->data().begin(),&pi->data());

    BOOST_CHECK( pi->data().begin() == p.i() );
    BOOST_CHECK( p.check(6u) );
    BOOST_CHECK( ! p.check(7u) );
    SENF_CHECK_NO_THROW( p.validate(6u) );
    BOOST_CHECK_THROW( p.validate(7u), senf::TruncatedPacketException );

    // ?? Why the heck do I need the +0? I get an 'undefined symbol FooParser::fixed_bytes'
    // otherwise ...
    BOOST_CHECK_EQUAL( FooParser::fixed_bytes+0, 6u );
    BOOST_CHECK_EQUAL( BarParser(pi->data().begin(),&pi->data()).bytes(), 6u );
    BOOST_CHECK_EQUAL( senf::bytes(senf::UInt16Parser(pi->data().begin(),&pi->data())), 2u );
    BOOST_CHECK_EQUAL( senf::bytes(FooParser(pi->data().begin(),&pi->data())), 6u );
    BOOST_CHECK_EQUAL( senf::bytes(BarParser(pi->data().begin(),&pi->data())), 6u );

    BOOST_CHECK_EQUAL( senf::init_bytes<FooParser>::value, 6u );
    BOOST_CHECK_EQUAL( senf::init_bytes<BarParser>::value, 6u );
}

SENF_AUTO_TEST_CASE(packetParser_insertion_operator)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi1 (senf::PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type( FooParser::fixed_bytes)));
    senf::PacketInterpreter<VoidPacket>::ptr pi2 (senf::PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type( FooParser::fixed_bytes)));
    FooParser p1 (pi1->data().begin(), &pi1->data());
    FooParser p2 (pi2->data().begin(), &pi2->data());
    std::generate(pi2->data().begin(), pi2->data().end(), UniqueNumber);
    p1 << p2;
    BOOST_CHECK( std::find_if(pi1->data().begin(), pi1->data().end(), IsUniqueNumber) == pi1->data().end() );
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
