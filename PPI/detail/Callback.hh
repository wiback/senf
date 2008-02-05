// $Id$
//
// Copyright (C) 2007
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
    \brief Callback public header */

#ifndef HH_Callback_
#define HH_Callback_ 1

// Custom includes
#include <boost/function.hpp>

//#include "Callback.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace detail {

    /** \brief Provide callbacks with a single optional argument

        This helper implements callbacks with an optional single argument. In addition to
        boost::function, this helper provides the following functionality:

        \li It allows the callback to ignore the argument: A callable with no argument may be used
            as callback.
        \li It allows to use member function pointers as callbacks. These will be automatically
            bound to the \a owner argument of \ref make().

        The callbacks follow the same restrictions as <a href="">Boost.Function</a>: They must be
        either function, member function pointers or callable objects defining the appropriate
        typedef members.
     */
    template <class Arg=void>
    struct Callback
    {
        typedef boost::function<void (Arg)> type;
        
        template <class Owner, class FnClass>
        static type make(void (FnClass::* memfn )(), Owner & owner);
        template <class Owner, class FnClass, class FnArg>
        static type make(void (FnClass::* memfn )(FnArg arg), Owner & owner);
        template <class Owner>
        static type make(type callable, Owner &);
        template <class Owner>
        static type make(boost::function<void()> callable, Owner &);
    };

#ifndef DOXYGEN

    template <>
    struct Callback<void>
    {
        typedef boost::function<void ()> type;
        
        template <class Owner, class FnClass>
        static type make(void (FnClass::* memfn )(), Owner & owner);
        template <class Owner>
        static type make(type callable, Owner &);
    };

#endif

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Callback.cci"
//#include "Callback.ct"
#include "Callback.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// End:
