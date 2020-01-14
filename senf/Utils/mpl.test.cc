//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief mpl unit tests */

#include "mpl.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(senfmpl)
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

SENF_AUTO_TEST_CASE(mplSlot)
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
