// $Id$
//
// Copyright (C) 2008 
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
    \brief String unit tests */

//#include "String.test.hh"
//#include "String.test.ih"

// Custom includes
#include "String.hh"
#include <iomanip>

#include "auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(stringJoin)
{
    char const * seq[] = { "foo", "bar", "baz" };
    BOOST_CHECK_EQUAL( senf::stringJoin(std::make_pair(seq, seq+sizeof(seq)/sizeof(seq[0])), "::"),
                       "foo::bar::baz" );
}

BOOST_AUTO_UNIT_TEST(lexicalCast)
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
