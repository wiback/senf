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
    \brief String unit tests */

#include "String.hh"

// Custom includes
#include <iomanip>

// Unit test includes
#include "auto_unit_test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(stringJoin)
{
    char const * seq[] = { "foo", "bar", "baz" };
    BOOST_CHECK_EQUAL( senf::stringJoin(std::make_pair(seq, seq+sizeof(seq)/sizeof(seq[0])), "::"),
                       "foo::bar::baz" );
}

SENF_AUTO_TEST_CASE(lexicalCast)
{
    SENF_CHECK_NO_THROW(
        BOOST_CHECK_EQUAL(
            senf::lexical_cast<unsigned>()[std::setbase(0)](std::string("0x1a2b")),
            6699u ) );
    SENF_CHECK_NO_THROW(
        BOOST_CHECK_EQUAL(
            senf::lexical_cast<std::string>()[std::hex][std::uppercase][std::showbase](6699u),
            "0X1A2B" ) );
    SENF_CHECK_NO_THROW(
        BOOST_CHECK_EQUAL(
            senf::lexical_cast<unsigned>(std::string("6699")), 6699u ) );
    SENF_CHECK_NO_THROW(
        BOOST_CHECK_EQUAL(
            senf::lexical_cast<std::string>(6699u), "6699" ) );
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
