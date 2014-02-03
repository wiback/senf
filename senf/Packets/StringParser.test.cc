// $Id$
//
// Copyright (C) 2009
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief StringParser.test unit tests */

//#include "StringParser.test.hh"
//#include "StringParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};
}

SENF_AUTO_UNIT_TEST(stringParser)
{
    typedef senf::StringParser<senf::UInt16Parser> MyStringParser;

    senf::PacketInterpreterBase::byte data[] = { 0x00, 0x04, 'T', 'E', 'S', 'T' };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));
    {
        BOOST_CHECK_EQUAL( p->data().size(), 6u );
        MyStringParser parser (p->data().begin(), &p->data());
        BOOST_CHECK_EQUAL( parser.length(), 4 );
        BOOST_CHECK_EQUAL( parser.value(), "TEST" );
        BOOST_CHECK(! parser.empty() );
        {
            std::string maxStr (std::string(MyStringParser::length_t::max_value, 'x'));
            parser.value( maxStr);
            BOOST_CHECK_EQUAL( parser.length(), maxStr.size() );
            BOOST_CHECK_EQUAL( parser.value(), maxStr );
            std::string tooLongStr (std::string(MyStringParser::length_t::max_value+1, 'x'));
            parser.value( tooLongStr);
            BOOST_CHECK_EQUAL( parser.length(), maxStr.size() );
            BOOST_CHECK_EQUAL( parser.value(), maxStr );
        }
    } {
        MyStringParser(p->data().begin(), &p->data()).value("Another Test");
        BOOST_CHECK_EQUAL( p->data().size(), 14u );
        BOOST_CHECK_EQUAL( p->data()[0], 0u );
        BOOST_CHECK_EQUAL( p->data()[1], 12u );
        BOOST_CHECK_EQUAL( MyStringParser(p->data().begin(), &p->data()).value(), "Another Test" );
        BOOST_CHECK_EQUAL( MyStringParser(p->data().begin(), &p->data()), "Another Test" );
        SENF_CHECK_NOT_EQUAL( MyStringParser(p->data().begin(), &p->data()), "Another Test 123" );
    } {
        MyStringParser(p->data().begin(), &p->data()).clear();
        BOOST_CHECK_EQUAL( p->data().size(), 2u );
        MyStringParser parser (p->data().begin(), &p->data());
        BOOST_CHECK_EQUAL( parser.length(), 0 );
        BOOST_CHECK_EQUAL( parser.value(), std::string() );
        BOOST_CHECK( parser.empty() );
    }
}

SENF_AUTO_UNIT_TEST(fixedSizeStringParser)
{
    typedef senf::FixedSizeStringParser<4> MyFixedSizeStringParser;

    senf::PacketInterpreterBase::byte data[] = { 'T', 'E', 'S', 'T' };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));

    BOOST_CHECK_EQUAL( p->data().size(), 4u );
    BOOST_CHECK_EQUAL( MyFixedSizeStringParser(p->data().begin(), &p->data()).value(), "TEST" );
    BOOST_CHECK_EQUAL( MyFixedSizeStringParser(p->data().begin(), &p->data()), "TEST" );

    MyFixedSizeStringParser(p->data().begin(), &p->data()).value("ab");
    std::string s = "ab"; s += '\0'; s += '\0';
    BOOST_CHECK_EQUAL(MyFixedSizeStringParser(p->data().begin(), &p->data()).value(), s);
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
