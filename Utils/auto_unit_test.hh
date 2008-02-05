// $Id$
//
// Copyright (C) 2007
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
    \brief Boost.Test auto unit test compatibility across Boost V 1.33 and 1.34

    This header file will allows to consistently use the Boost Version 1.33 syntax for defining auto
    unit tests. If the available Boost version is 1.34, this file will automatically take care of
    any necessary workarounds.

    So, instead of <tt>\#include <boost/test/auto_unit_test.hpp></tt>, you should always write
    <tt>\#include "../Utils/auto_unit_test.hh"</tt> (with possibliy adjusted path).
 */

#ifndef HH_auto_unit_test_
#define HH_auto_unit_test_ 1

// Custom includes
#include <boost/version.hpp>

//#include "auto_unit_test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#if BOOST_VERSION >= 103400

#   define BOOST_AUTO_UNIT_TEST BOOST_AUTO_TEST_CASE

#endif

#include <boost/test/auto_unit_test.hpp>

///////////////////////////////hh.e////////////////////////////////////////
//#include "auto_unit_test.cci"
//#include "auto_unit_test.ct"
//#include "auto_unit_test.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
