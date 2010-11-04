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

#ifndef HH_SENF_Utils_type_traits_
#define HH_SENF_Utils_type_traits_ 1

// Custom includes
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_function.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/bind.hpp>
#include <senf/config.hh>

#include "type_traits.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf
{

    /** \defgroup typetraits Type traits
     */

    ///\addtogroup typetraits
    //\{

    /** \brief Strip first parameter from function traits

        This meta-function will remove the first argument from \a Traits. The return value is a new
        function trait with one less argument.

        If the function described in \a Traits does not take any arguments, it is returned
        unchanged.

        \code
        typedef boost::function_traits<void (int, double)> traits
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::function_traits_remove_arg< traits >::type,
            boost::function_traits<void (double)>
        >::value ));
        \endcode

        \tparam Traits \c boost::function_traits instantiation
     */
    template < class Traits, unsigned arity = Traits::arity >
    struct function_traits_remove_arg {};

    /** \brief Get argument type from function traits

        function_traits_arg<Traits, index> will return the type of the \a index-th argument from \a
        Traits. If the function has no argument at that index, \c void is returned

        \code
        typedef boost::function_traits<void (int, double)> traits;
        BOOST_STATIC_ASSERT(( boost:is_same<
            senf::function_traits_arg_type< traits, 0 >::type,
            int
        >::value ));
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::function_traits_arg_type< traits, 2 >::type,
            void
        >::value ));
        \endcode

        \tparam Traits \c boost::function_traits instantiation
        \tparam index 0 based argument index
     */
    template < class Traits, int index, bool flag = (index < Traits::arity) >
    struct function_traits_arg_type {};

#ifndef DOXYGEN

    template < class Traits, int index >
    struct function_traits_arg_type <Traits, index, false >
    {
        typedef void type;
    };

#endif

    /** \brief Remove member pointer from type

        This meta function will remove any type of member pointer from it's argument. Other types
        will be returned unchanged.

        \code
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_member_pointer< int (Class::*) >::type,
            int
        >::value ));
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_member_pointer< void (Class::*)(int) >::type,
            void (int)
        >::value ));
        \endcode

        \tparam MemberPointer type to remove member pointer from
     */
    template < class MemberPointer > struct remove_member_pointer
    {
        typedef MemberPointer type;
    };

#ifndef DOXYGEN

    template < class C, class T > struct remove_member_pointer <T (C::*) >
    {
        typedef T type;
    };

    template < class C, class T > struct remove_member_pointer <T (C::* const) >
    {
        typedef T type;
    };

#endif

    /** \brief Get class of a member pointer

        Returns the class, an arbitrary member pointer belongs to. If the argument is not a member
        pointer, void is returned.

        \code
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::member_class< int (Class::*) >::type,
            Class
        >::value ));
        BOOST_STATIC_ASSERT(( boost::is_Same<
            senf::member_class< int * >::type,
            void
        >::value ));
        \endcode

        \tparam MemberPointer Type to get the member pointer class from
     */
    template < class MemberPointer > struct member_class
    {
        typedef void type;
    };

#ifndef DOXYGEN

    template < class C, class T > struct member_class <T (C::*) >
    {
        typedef C type;
    };

    template < class C, class T > struct member_class <T (C::* const) >
    {
        typedef C type;
    };

#endif

    /** \brief Remove any type of pointer from type

        This meta function will remove a plain or member pointer from the given type. If \a T is
        neither a member pointer nor an ordinary pointer, \a T will be returned unchanged.

        \code
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_any_pointer< int (Class::*) >::type,
            int
        >::value ));
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_any_pointer< void (Class::*)(int) >::type,
            void (int) > );
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_any_pointer < int (*)() >::type,
            int (
        >::value ));
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_any_pointer < int >::type,
            int
        >::value ));
        \endcode

        \tparam T type to remove member pointer from
     */
    template < class T >
    struct remove_any_pointer
        : public remove_member_pointer < typename boost::remove_pointer < T >::type >
    {};

    /** \brief Test object if it is a function or member-function (pointer)

        is_any_function will inherit from \c boost::true_type, when \a T is a function type,
        function pointer type or a member function pointer type. Otherwise, it will inherit from \c
        boost::false_type.

        \code
        BOOST_STATIC_ASSERT((   senf::is_any_function< void () >::value ));
        BOOST_STATIC_ASSERT((   senf::is_any_function< void (*)(int) >::value ));
        BOOST_STATIC_ASSERT((   senf::is_any_function< void (Class::*)() >::value ));
        BOOST_STATIC_ASSERT(( ! senf::is_any_function< int * >::value ));
        \endcode

        \tparam T type to test
     */
    template < class T >
    struct is_any_function
        : public boost::is_function < typename senf::remove_any_pointer < T >::type >
    {};

    /** \brief Remove reference and CV qualification from type

        remove_cvref will remove all the 'ornaments' from a type as typically used to pass
        arguments: references and any CV spec. It will thus convert a typical argument type into
        it's basic type.

        \code
        BOOST_STATIC_ASSERT(( boost::is_same<
            senf::remove_cvref<int const &>::type,
            int
        >::value ));
        \endcode
     */
    template < class T >
    struct remove_cvref
        : public boost::remove_cv< typename boost::remove_reference<T>::type >
    {};

    /** \brief Get arity of function T

        \a T may be any function like type: function, pointer to function or (pointer to)
        member-function.

        \code
        BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)(int)>::value == 1 ));
        \endcode
     */
    template < class T >
    struct function_arity
        : public boost::integral_constant<
              unsigned,
              boost::function_traits<
                  typename senf::remove_any_pointer<T>::type>::arity>
    {};

    /** Test object if it is any \c std::pair type

        if \a Pair is any \c std::pair type, this trait will inherit from \c boost::true_type,
        otherwise it will inherit from \c boost::false_type.

        \code
        BOOST_STATIC_ASSERT((   senf::is_pair< std::pair<int,void*> >::value ));
        BOOST_STATIC_ASSERT(( ! senf::is_pair< void () >::value ));
        \endcode
     */
    template <class Pair>
    struct is_pair
        : public boost::false_type
    {};

#ifndef DOXYGEN
    template <class First, class Second>
    struct is_pair< std::pair<First,Second> >
        : public boost::true_type
    {};
#endif

    //\}

#ifndef DOXYGEN

#   define BOOST_PP_ITERATION_PARAMS_1 (4, (0, 10,                                                \
                                            SENF_ABSOLUTE_INCLUDE_PATH(Utils/type_traits.mpp),    \
                                            1))
#   include BOOST_PP_ITERATE()

#endif

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
