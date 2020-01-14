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
    \brief Terminfo unit tests */

#include "Terminfo.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(terminfo)
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
