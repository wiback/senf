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
