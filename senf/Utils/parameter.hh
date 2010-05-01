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
    \brief parameter public header */

/** \defgroup boost_parameter Boost.Parameter utilities

    Here we define some utilities for the <a
    href="http://www.boost.org/doc/libs/1_33_1/libs/parameter/doc/html/index.html">Boost.Parameter</a>
    library.
 */

#ifndef HH_SENF_Utils_parameter_
#define HH_SENF_Utils_parameter_ 1

// Custom includes
#include <boost/parameter/binding.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>

//#include "parameter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    ///\addtogroup boost_parameter
    ///\{

    /** \brief Test \a ArgumentPack, whether the \a TagType argument was supplied

        This check will test, whether the argument identified by the \a TagType keyword tag type was
        specified in the \a ArgumentPack. This inherits from \c boost::mpl::true_ or \c
        boost::mpl::false_ accordingly.

        \tparam ArgumentPack argument pack to check
        \tparam TagType keyword tag type identifying the argument to check
     */
    template <class ArgumentPack, class TagType>
    struct has_parameter
        : public boost::mpl::not_<
              boost::is_same< typename boost::parameter::binding< ArgumentPack, TagType, void>::type,
                                                                  void > >::type
    {};

    ///\}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "parameter.cci"
//#include "parameter.ct"
//#include "parameter.cti"
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
