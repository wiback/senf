// $Id$
//
// Copyright (C) 2008
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
    \brief String unit tests */

//#include "String.test.hh"
//#include "String.test.ih"

// Custom includes
#include "String.hh"
#include <iomanip>

#include "auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(stringJoin)
{
    char const * seq[] = { "foo", "bar", "baz" };
    BOOST_CHECK_EQUAL( senf::stringJoin(std::make_pair(seq, seq+sizeof(seq)/sizeof(seq[0])), "::"),
                       "foo::bar::baz" );
}

SENF_AUTO_UNIT_TEST(lexicalCast)
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
