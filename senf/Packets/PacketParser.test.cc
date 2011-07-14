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
    \brief PacketParser unit tests */

//#include "PacketParser.test.hh"
//#include "PacketParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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
}

SENF_AUTO_UNIT_TEST(packetParserBase)
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
