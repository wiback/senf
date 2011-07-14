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
    \brief parameter public header */

/** \defgroup boost_parameter Boost.Parameter utilities

    Here we define some utilities for the <a
    href="http://www.boost.org/doc/libs/release/libs/parameter/doc/html/index.html">Boost.Parameter</a>
    library.
 */

#ifndef HH_SENF_Utils_parameter_
#define HH_SENF_Utils_parameter_ 1

// Custom includes
#include <boost/parameter/binding.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>

//#include "parameter.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    ///\addtogroup boost_parameter
    //\{

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

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
