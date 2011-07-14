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

/** \file
    \brief Terminfo unit tests */

//#include "Terminfo.test.hh"
//#include "Terminfo.test.ih"

// Custom includes
#include "Terminfo.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    typedef std::pair<senf::term::KeyParser::keycode_t, senf::term::KeyParser::size_type> Pair;
}

namespace std {

    std::ostream & operator<<(std::ostream & os, Pair const & pair)
    {
        os << '(' << pair.first << ',' << pair.second << ')';
        return os;
    }
}

SENF_AUTO_UNIT_TEST(terminfo)
{
    senf::term::Terminfo ifo ("vt220");
    senf::term::KeyParser kp (ifo);

    //ifo.dump(std::cout);
    //kp.dump(std::cout);

    BOOST_CHECK_EQUAL( kp.lookup("\e[5\x7e"),
                       Pair(senf::term::KeyParser::PageUp, 4u) );
    BOOST_CHECK_EQUAL( kp.lookup("\e"),
                       Pair(senf::term::KeyParser::Incomplete, 1u) );
    BOOST_CHECK_EQUAL( kp.lookup("\e["),
                       Pair(senf::term::KeyParser::Incomplete, 2u) );
    BOOST_CHECK_EQUAL( kp.lookup("\e[A"),
                       Pair(senf::term::KeyParser::Up, 3u) );
    BOOST_CHECK_EQUAL( kp.lookup("\e[\e"),
                       Pair(senf::term::KeyParser::keycode_t('\e'), 1u) );
    BOOST_CHECK_EQUAL( kp.lookup("a\e[Ab"),
                       Pair(senf::term::KeyParser::keycode_t('a'), 1u) );
    BOOST_CHECK_EQUAL( kp.lookup("\e[Ab"),
                       Pair(senf::term::KeyParser::Up, 3u) );
    BOOST_CHECK_EQUAL( kp.lookup("b"),
                       Pair(senf::term::KeyParser::keycode_t('b'), 1u) );
    BOOST_CHECK_EQUAL( kp.lookup(""),
                       Pair(senf::term::KeyParser::keycode_t('\0'), 0u) );

    BOOST_CHECK_EQUAL( ifo.formatString(senf::term::Terminfo::properties::CursorAddress,
                                        9, 12),
                       "\e[10;13H" );
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
