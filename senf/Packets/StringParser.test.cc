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
    \brief StringParser.test unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};
}

SENF_AUTO_TEST_CASE(stringParser)
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

SENF_AUTO_TEST_CASE(fixedSizeStringParser)
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
