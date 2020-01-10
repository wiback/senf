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
    \brief Tags public header */

#ifndef HH_SENF_Utils_Tags_
#define HH_SENF_Utils_Tags_ 1

// Custom includes

//#include "Tags.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup utils_tags Type tags

    Type tags are used to select variants of a member, function, template or constructor. A Type tag
    always has only a single value which is passed wherever a parameter of that tag's type is
    needed.

    The argument which expects a type-tag value will always be declared in such a way, that it is
    optional. Examples of type tag use are:

    \code
    result = object.find<Foo>();                  // This call may throw
    result = object.find<Foo>(senf::nothrow);     // This call will not throw
    \endcode

    Of course, this only works with objects which explicitly declare, that they take an optional
    senf::NoThrow_t type parameter.
 */

namespace senf {

    /** \brief Type tag selecting non-throwing variant of something

        This tag is used to select the non-throwing variant of a member, function, template or
        constructor. An argument of this type is always declared in such a way, that it is
        optional.

        There is only a single value for this type: \ref senf::nothrow which is the value to pass
        wherever an (optional) senf::NoThrow_t parameter is requested.

        \ingroup utils_tags
     */
    enum NoThrow_t { nothrow };

    /** \brief Type tag selecting uninitialized variant of something

        This tag is used to select the unititialized variant of a member, function, template or
        constructor (mostly a constructor). An argument of this type is always declared in such a
        way, that it is optional.

        There is only a single value for this type: \ref senf::noinit which is the value to pass
        wherever an (optional) senf::NoInit_t parameter is requested.

        \ingroup utils_tags
     */
    enum NoInit_t { noinit };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Tags.cci"
//#include "Tags.ct"
//#include "Tags.cti"
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
