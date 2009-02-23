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
    <pre>
    \#include "../Utils/auto_unit_test.hh"
    </pre> (with possibliy adjusted path).
 */

#ifndef HH_SENF_Utils_auto_unit_test_
#define HH_SENF_Utils_auto_unit_test_ 1

// Custom includes
#include <iostream>
#include <boost/version.hpp>

//#include "auto_unit_test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#if BOOST_VERSION >= 103400

#   define BOOST_AUTO_UNIT_TEST BOOST_AUTO_TEST_CASE

#endif

#include <boost/test/auto_unit_test.hpp>

/** \defgroup unittest Boost.Test extensions

    This module defines some additional extensions to Boost.Test
 */

/** \brief Check for compile failure

    COMPILE_FAIL() is used to check, that a certain piece of code will produce a compile time
    failure.

    \code
    #ifdef COMPILE_CHECK

    COMPILE_FAIL(foo)
    {
        // fails to compile ....
        int x = "foo";
    }
    
    COMPILE_FAIL(bar)
    { ... }

    #endif
    \endcode

    This check is performed by the extended unit-test builder in \c senfscons.
    
    \ingroup unittest
 */
#define COMPILE_FAIL(n) void n()

/** \brief Show exception information

    \ref SENF_CHECK_NO_THROW() is an extension to \c BOOST_CHECK_NO_THROW() which will write out the
    unabridged exception information when an exception is thrown in addition to signaling an error.

    \ingroup unittest
 */
#define SENF_CHECK_NO_THROW(expr)                                                                 \
    BOOST_CHECK_NO_THROW(                                                                         \
        try { expr ; }                                                                            \
        catch (std::exception & e) { std::cerr << e.what() << std::endl; throw; } )

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
