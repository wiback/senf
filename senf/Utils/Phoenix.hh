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
