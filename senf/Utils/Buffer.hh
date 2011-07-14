// $Id$
//
// Copyright (C) 2007
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
    \brief Buffer public header */

#ifndef HH_SENF_Utils_Buffer_
#define HH_SENF_Utils_Buffer_ 1

// Custom includes
#include <senf/config.hh>

//#include "Buffer.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(SENF_BUFFER_USE_LOCALS)

#   define SENF_SCOPED_BUFFER(type, sym, size)  \
        type sym[size];

#elif defined(SENF_BUFFER_USE_ALLOCA)

#   include <alloca.h>
#   define SENF_SCOPED_BUFFER(type, sym, size)                          \
        type * sym (static_cast<type *>(alloca(size*sizeof(type))));

#elif defined(SENF_BUFFER_USE_NEW)

#   include <boost/scoped_array.hpp>
    /** \brief Allocate a local buffer

        SENF_SCOPED_BUFFER will allocate a local variable named \a sym as a buffer of \a size elements
        of type \a type. The buffer will \e not be initialized in any way and \a type must be a POD
        type.

        This macro is used when \a size is a dynamic expression and not a constant value. For constant
        values, use \c boost::array (or C++ builtin arrays). Depending on compiler support, this version
        will try to avoid dynamic memory allocation. The type of the declared local variable \a sym is
        either pointer to \a type or array of \a type:

        \code
          #include "../Utils/Buffer.hh"

          void foo(std::string const & str)
          {
              // create temp copy of str (like c_str() member)
              SENF_SCOPED_BUFFER(char, buf, str.size()+1);
              *std::copy(str.begin(), str.end(), buf) = 0;

              // use buf ...
          }
        \endcode

        \param type Type of buffer element
        \param sym Name of local symbol
        \param size size of the Area to allocate.

        \hideinitializer
     */
#   define SENF_SCOPED_BUFFER(type, sym, size)                                                          \
        boost::scoped_array<type> _senf_scoped_buffer__ ## sym ## __ ## __LINE__ (new type[size]);      \
        type * sym (_senf_scoped_buffer__ ## sym ## __ ## __LINE__.get());

#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Buffer.cci"
//#include "Buffer.ct"
//#include "Buffer.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
