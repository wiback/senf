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
    \brief Phoenix public header */

#ifndef HH_SENF_Utils_Phoenix_
#define HH_SENF_Utils_Phoenix_ 1

// Custom includes
#include <senf/config.hh>

#if HAVE_BOOST_SPIRIT_INCLUDE_CLASSIC_HPP
#  include <boost/spirit/include/phoenix1_functions.hpp>
#else
#  include <boost/spirit/phoenix/functions.hpp>
#endif

//#include "Phoenix.mpp"
#include "Phoenix.ih"
///////////////////////////////hh.p////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
