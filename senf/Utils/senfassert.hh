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
    \brief senfassert public header */

#ifndef HH_SENF_Utils_senfassert_
#define HH_SENF_Utils_senfassert_ 1

// Custom includes

//#include "senfassert.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SENF_DEBUG

#   include "IgnoreValue.hh"
#   define SENF_ASSERT(x, comment)
#   define SENF_ASSERT_EXPRESSION(expression, comment) senf::IGNORE( expression )

#else

#ifndef SENF_ASSERT_EXCEPTION

#   include <cassert>
#   define SENF_ASSERT(x, comment) assert((x) && comment)
#   define SENF_ASSERT_EXPRESSION(expression, comment) assert((expression) && comment)

#else

#   include "Exception.hh"
    namespace senf {
        struct AssertException : public Exception
        {
            AssertException(std::string const & descr) {
                (*this) << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ": " << descr;
            }
        };
    }

#   define SENF_ASSERT(x, comment) if (!(x)) throw senf::AssertException(comment);
#   define SENF_ASSERT_EXPRESSION(expression, comment) if (!(expression)) throw senf::AssertException(comment);

#endif
#endif


/* Lifted direct from:
   Modern C++ Design: Generic Programming and Design Patterns Applied Section 2.1
   by Andrei Alexandrescu
*/
namespace senf {
namespace detail {
    template<bool> class compile_time_check
    {
    public:
        compile_time_check(...) {}
    };

    template<> class compile_time_check<false>
    {
    };
}}

    /*
    SENF_STATIC_ASSERT is only in operation when SENF_DEBUG is defined. It will test its first
    argument at compile time and on failure report the error message of the second argument,
    which must be a valid c++ classname. i.e. no spaces, punctuation or reserved keywords.
    */
#ifdef SENF_DEBUG
#   define SENF_STATIC_ASSERT(expr, msg)                                    \
    do {                                                                    \
        struct STATIC_ASSERT_FAILED_##msg {};                               \
        typedef senf::detail::compile_time_check< (expr) != 0 > tmplimpl;   \
        tmplimpl aTemp = tmplimpl(STATIC_ASSERT_FAILED_##msg());            \
        (void)sizeof(aTemp);                                                      \
    } while (0)
#else
#   define SENF_STATIC_ASSERT(expr, msg)
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "senfassert.cci"
//#include "senfassert.ct"
//#include "senfassert.cti"
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
