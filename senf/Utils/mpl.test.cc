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
    \brief mpl unit tests */

//#include "mpl.test.hh"
//#include "mpl.test.ih"

// Custom includes
#include "mpl.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    template <unsigned _>
    struct select {
        static bool const has_int_value = false;
        static bool const has_class_value = false;
    };
    template <class T>
    senf::mpl::rv<0> select_(...);


    template <>
    struct select<1> {
        static bool const has_int_value = true;
        static bool const has_class_value = false;
    };
    template <class T>
    senf::mpl::rv<1> select_(int, senf::mpl::take_int<T::value> * = 0);

    template <>
    struct select<2> {
        static bool const has_int_value = false;
        static bool const has_class_value = true;
    };
    template <class T>
    senf::mpl::rv<2> select_(int, senf::mpl::take_class<typename T::value> * = 0);

    template <class T>
    struct choice : public select<SENF_MPL_RV( select_<T>(0) )> {};

    struct A { static const int value = 0; };
    struct B { struct value {}; };
    struct C {};
}

SENF_AUTO_UNIT_TEST(senfmpl)
{
    BOOST_CHECK( choice<A>::has_int_value );
    BOOST_CHECK( ! choice<A>::has_class_value );

    BOOST_CHECK( ! choice<B>::has_int_value );
    BOOST_CHECK( choice<B>::has_class_value );

    BOOST_CHECK( ! choice<C>::has_int_value );
    BOOST_CHECK( ! choice<C>::has_class_value );
}

namespace {

#   define Begin() SENF_MPL_SLOT_DEF(accum, 0)
#   define Add(n) SENF_MPL_SLOT_SET(accum, SENF_MPL_SLOT_GET(accum)+n)
#   define End() BOOST_STATIC_CONSTANT(unsigned long, total = SENF_MPL_SLOT_GET(accum))

    struct Test
    {
        Begin();

        Add(2);
        Add(2);

        // Here some comment ...

        Add(4);

        End();
    };

}

SENF_AUTO_UNIT_TEST(mplSlot)
{
    BOOST_CHECK_EQUAL( unsigned(Test::total), 8u );
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
