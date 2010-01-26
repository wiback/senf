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
    \brief mpl unit tests */

//#include "mpl.test.hh"
//#include "mpl.test.ih"

// Custom includes
#include "mpl.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
#   define End() BOOST_STATIC_CONSTANT(unsigned, total = SENF_MPL_SLOT_GET(accum))

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

///////////////////////////////cc.e////////////////////////////////////////
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
