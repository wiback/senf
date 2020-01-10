//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Phoenix public header */

#ifndef HH_SENF_Utils_Phoenix_
#define HH_SENF_Utils_Phoenix_ 1

// Custom includes
#include <boost/spirit/include/phoenix1_functions.hpp>

//#include "Phoenix.mpp"
#include "Phoenix.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup phoenix_helpers Boost.Phoenix extensions */

namespace senf {
namespace phoenix {

    /** \brief \c push_back phoenix functor

        <tt>push_back(c, x) := c.push_back(x)</tt>

        \ingroup phoenix_helpers
     */
    ::phoenix::function<detail::push_back> const push_back;

    /** \brief \c clear phoenix functor

        <tt>clear(c) := c.clear()</tt>

        \ingroup phoenix_helpers
     */
    ::phoenix::function<detail::clear>     const clear;

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Phoenix.cci"
//#include "Phoenix.ct"
//#include "Phoenix.cti"
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
