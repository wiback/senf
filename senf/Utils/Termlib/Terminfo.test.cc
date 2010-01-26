// $Id$
//
// Copyright (C) 2009 
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
    \brief Terminfo unit tests */

//#include "Terminfo.test.hh"
//#include "Terminfo.test.ih"

// Custom includes
#include "Terminfo.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
