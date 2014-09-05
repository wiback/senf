// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief Boost.Test extensions
 */

#ifndef HH_SENF_Utils_auto_unit_test_
#define HH_SENF_Utils_auto_unit_test_ 1

// Custom includes
#include <iostream>

#include "auto_unit_test.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

/** \defgroup unittest Boost.Test extensions

    This module defines some additional extensions to Boost.Test
 */

/** \brief Check for compile failure

    \c COMPILE_FAIL() is used to check, that a certain piece of code will produce a compile
    time failure.

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

    \c SENF_CHECK_NO_THROW() is an extension to \c BOOST_CHECK_NO_THROW() which will write out the
    unabridged exception information when an exception is thrown in addition to signaling an error.

    \ingroup unittest
 */
#define SENF_CHECK_NO_THROW(expr)                                                               \
    BOOST_CHECK_NO_THROW(                                                                       \
        try { expr ; }                                                                          \
        catch (std::exception & e) { std::cerr << e.what() << std::endl; throw; } )

#define SENF_REQUIRE_NO_THROW(expr)                                                             \
    BOOST_REQUIRE_NO_THROW(                                                                     \
        try { expr ; }                                                                          \
        catch (std::exception & e) { std::cerr << e.what() << std::endl; throw; } )

namespace senf {
namespace test {

    template <class Iterator>
    typename detail::NoCharIterator<Iterator>::type nocharIterator(Iterator i);

}}

/** \brief Check ranges for equality, writing char's as int's

    \c SENF_CHECK_EQUAL_COLLECTIONS() is like \c BOOST_CHECK_EQUAL_COLLECTIONS(). The only
    difference is, that \c SENF_CHECK_EQUAL_COLLECTIONS() will write out collection values in
    numeric form (and not as single characters) if the elements are of any char type. Other types
    are not affected.

    \hideinitializer
    \ingroup unittest
 */
#define SENF_CHECK_EQUAL_COLLECTIONS(left_begin, left_end, right_begin, right_end)              \
    BOOST_CHECK_EQUAL_COLLECTIONS(                                                              \
        senf::test::nocharIterator(left_begin), senf::test::nocharIterator(left_end),           \
        senf::test::nocharIterator(right_begin), senf::test::nocharIterator(right_end))

/** \brief Define new test case

    \c SENF_AUTO_UNIT_TEST() is like \c BOOST_AUTO_UNIT_TEST(). The only difference is, that
    if an exception is thrown in the test case \c SENF_AUTO_UNIT_TEST() will write out the
    complete what-message of the exception.

    \hideinitializer
    \ingroup unittest
 */
#define SENF_AUTO_TEST_CASE(name)                                                               \
    void senf_test_body_##name();                                                               \
    BOOST_AUTO_TEST_CASE(name)                                                                  \
    {                                                                                           \
        try {                                                                                   \
            senf_test_body_##name();                                                            \
        }                                                                                       \
        catch (std::exception & e) {                                                            \
            std::cerr << e.what() << std::endl;                                                 \
            throw;                                                                              \
        }                                                                                       \
    }                                                                                           \
    void senf_test_body_##name()


#ifdef DOXYGEN
    /** \brief Check for non-equality

        \c SENF_CHECK_NOT_EQUAL() is the opposite of \c BOOST_CHECK_EQUAL.
        \hideinitializer
        \ingroup unittest
     */
    #define SENF_CHECK_NOT_EQUAL( Left, Rigth)
#else
#ifdef BOOST_CHECK_NE
    #define SENF_CHECK_NOT_EQUAL( Left, Right) BOOST_CHECK_NE( Left, Right)
#else
    #define SENF_CHECK_NOT_EQUAL( Left, Right) BOOST_CHECK( Left != Right)
#endif
#endif

#define SENF_RETURN_NO_ROOT_PRIVILEGES(msg)                                                     \
    if (getuid() != 0) {                                                                        \
        BOOST_WARN_MESSAGE(false, msg);                                                         \
        BOOST_CHECK(true);                                                                      \
        return;                                                                                 \
    }

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "auto_unit_test.cci"
//#include "auto_unit_test.ct"
#include "auto_unit_test.cti"
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
