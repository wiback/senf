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
