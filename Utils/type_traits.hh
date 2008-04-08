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
    \brief type_traits public header */

#ifndef HH_type_traits_
#define HH_type_traits_ 1

// Custom includes
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_function.hpp>
#include <boost/bind.hpp>

#include "type_traits.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class Traits, unsigned arity=Traits::arity>
    struct function_traits_remove_arg {};

    template <class MemberPointer>
    struct remove_member_pointer
    {
        typedef MemberPointer type;
    };

    template <class C, class T>
    struct remove_member_pointer<T (C::*)>
    {
        typedef T type;
    };

    template <class MemberPointer>
    struct member_class
    {
        typedef void type;
    };

    template <class C, class T>
    struct member_class<T (C::*)>
    {
        typedef C type;
    };

    template <class T>
    struct remove_any_pointer
        : public remove_member_pointer<typename boost::remove_pointer<T>::type>
    {};

    template <class T>
    struct is_any_function
        : public boost::is_function<typename senf::remove_any_pointer<T>::type>
    {};

#   define BOOST_PP_ITERATION_PARAMS_1 (4, (0, 10,                                                 \
                                            SENF_ABSOLUTE_INCLUDE_PATH(Utils/type_traits.mpp), \
                                            1))
#   include BOOST_PP_ITERATE()

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "type_traits.cci"
//#include "type_traits.ct"
//#include "type_traits.cti"
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
