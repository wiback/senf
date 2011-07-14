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
    \brief type_traits unit tests */

//#include "type_traits.test.hh"
//#include "type_traits.test.ih"

// Custom includes
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include "type_traits.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct Class;
}

SENF_AUTO_UNIT_TEST(typeTraits)
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
