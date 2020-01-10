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
    \brief type_traits unit tests */

#include "type_traits.hh"

// Custom includes
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct Class;
}

SENF_AUTO_TEST_CASE(typeTraits)
{
    typedef boost::function_traits<void (int, double)> traits;

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::function_traits_remove_arg< traits >::type,
        boost::function_traits<void (double)>
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::function_traits_arg_type< traits, 0 >::type,
        int
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::function_traits_arg_type< traits, 2 >::type,
        void
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< Class (Class::*) >::type,
        Class
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< Class const (Class::*) >::type,
        Class const
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< Class (Class::*)(int) >::type,
        Class (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< Class const (Class::*)(int) >::type,
        Class const (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< void (Class::*)(int) const>::type,
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< Class const (Class::*)(int) const>::type,
        Class const (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< void (Class::* const)(int)>::type,
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_member_pointer< void (Class::* const)(int) const>::type,
        void (int)
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::member_class< int (Class::*) >::type,
        Class
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::member_class< int * >::type,
        void
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer< int (Class::*) >::type,
        int
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer< void (Class::*)(int) >::type,
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer < int (*)() >::type,
        int ()
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer < int >::type,
        int
    >::value ));

    BOOST_STATIC_ASSERT((   senf::is_any_function< void () >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (*)(int) >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (Class::*)() >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (Class::*)() const >::value ));
    BOOST_STATIC_ASSERT(( ! senf::is_any_function< int * >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_cvref<int const &>::type,
        int
    >::value ));

    BOOST_STATIC_ASSERT(( senf::function_arity<void ()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (int,int)>::value == 2 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (*)()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (*)(int,int)>::value == 2 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)(int,int)>::value == 2 ));

    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)(int)>::value == 1 ));

    BOOST_STATIC_ASSERT((   senf::is_pair< std::pair<int,void*> >::value ));
    BOOST_STATIC_ASSERT(( ! senf::is_pair< void () >::value ));

    BOOST_CHECK( true );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
