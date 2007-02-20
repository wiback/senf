// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief membind  public header */

/** \defgroup membind Bound Member Functions

    The membind() family of function templates simplifies the creation
    of simple bound member function pointers:

    \code
      struct Foo {
          int test(int x);
      };

      Foo * foo = ...;
      boost::function<int (int)> f = senf::membind(&Foo::test,foo);
      int rv = f(1); // Calls foo->test(1)
    \endcode

    \idea Make the \a ob argument type an additional P template
    parameter (using call_traits for the exact arg type? Probably
    we'll get deduction problems then) . The only operation this
    object musst suppoprt is ob->*fn. This would allow the use of
    smart pointers. We should keep the T & version to still support
    ob.*fn use.
 */

#ifndef HH_membind_
#define HH_membind_ 1

// Custom includes
#include <boost/bind.hpp>
#include <boost/function.hpp>

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#define scOBTYPE T *
#include "Utils/impl/membind.hh"
#undef scOBTYPE

#define scOBTYPE T &
#include "Utils/impl/membind.hh"
#undef scOBTYPE

#ifdef DOXYGEN

    /// \addtogroup membind
    /// @{

    /** \brief Build bound member function object

        membind() supports up to 9 function parameters (represented as
        \a Args here). The \a ob argument can be either a pointer or a
        reference to \a T
        \param[in] fn member function pointer
        \param[in] ob object instance to bind this pointer to
        \returns Boost.Function object representing a bound call of \a
            fn on \a ob
     */
    template <typename R, typename T, typename Args>
    boost::function<R (Args)> membind(R (T::* fn)( Args ), T * ob);

    /// @}

#endif

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "membind.cci"
//#include "membind.ct"
//#include "membind.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
