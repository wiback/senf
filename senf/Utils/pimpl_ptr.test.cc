// $Id$
//
// Copyright (C) 2010
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
    \brief pimpl_ptr.test unit tests */

//#include "pimpl_ptr.test.hh"
//#include "pimpl_ptr.test.ih"

// Custom includes
#include "pimpl_ptr.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct PimplTest
    {
        struct Impl;
        senf::pimpl_ptr<Impl> impl_;

        explicit PimplTest(int v);
    };

    unsigned implConstruct (0);
    unsigned implClone (0);
    unsigned implDestroy (0);

    struct PimplTest::Impl
    {
        Impl(int v) : value (v) { ++ implConstruct; }
        Impl(Impl const & other) : value (other.value) { ++ implClone; }
        ~Impl() { ++ implDestroy; }

        int value;
    };

    prefix_ PimplTest::PimplTest(int v)
        : impl_ (new Impl(v))
    {}

}

BOOST_AUTO_UNIT_TEST(pimpl_ptr)
{
    {
        PimplTest ob1 (1);
        PimplTest ob2 (ob1);
        PimplTest ob3 (3);

        BOOST_CHECK_EQUAL( implConstruct, 2u );
        BOOST_CHECK_EQUAL( implClone, 1u );
        BOOST_CHECK_EQUAL( implDestroy, 0u );

        BOOST_CHECK( ob1.impl_.get() != ob2.impl_.get() );
        BOOST_CHECK( ob2.impl_.get() != ob3.impl_.get() );
        BOOST_CHECK( ob3.impl_.get() != ob1.impl_.get() );

        BOOST_CHECK_EQUAL( ob1.impl_->value, 1 );
        BOOST_CHECK_EQUAL( ob2.impl_->value, 1 );
        BOOST_CHECK_EQUAL( ob3.impl_->value, 3 );

        ob3 = ob1;

        BOOST_CHECK_EQUAL( implConstruct, 2u );
        BOOST_CHECK_EQUAL( implClone, 2u );
        BOOST_CHECK_EQUAL( implDestroy, 1u );

        BOOST_CHECK( ob3.impl_.get() != ob1.impl_.get() );

        BOOST_CHECK_EQUAL( ob1.impl_->value, 1 );
        BOOST_CHECK_EQUAL( ob3.impl_->value, 1 );

        ob3.impl_->value = 3;

        BOOST_CHECK_EQUAL( ob1.impl_->value, 1 );
        BOOST_CHECK_EQUAL( ob3.impl_->value, 3 );

        struct PimplTest::Impl * p1 (ob1.impl_.get());
        struct PimplTest::Impl * p3 (ob3.impl_.get());

        std::swap(ob3.impl_, ob1.impl_);

        BOOST_CHECK_EQUAL( implConstruct, 2u );
        BOOST_CHECK_EQUAL( implClone, 2u );
        BOOST_CHECK_EQUAL( implDestroy, 1u );

        BOOST_CHECK_EQUAL( ob1.impl_->value, 3 );
        BOOST_CHECK_EQUAL( ob3.impl_->value, 1 );

        BOOST_CHECK_EQUAL( ob1.impl_.get(), p3 );
        BOOST_CHECK_EQUAL( ob3.impl_.get(), p1 );
    }

    BOOST_CHECK_EQUAL( implConstruct, 2u );
    BOOST_CHECK_EQUAL( implClone, 2u );
    BOOST_CHECK_EQUAL( implDestroy, 4u );
}

#ifdef COMPILE_CHECK

namespace {

    // We need this template arg to delay the constructor instantiation
    // until within COMPILE_FAIL. Otherwise the CompileCheck builder has no
    // way to associate the error message with the test ...
    template <class Delay=void>
    struct PimplTestIncomplete
    {
        struct Impl;
        senf::pimpl_ptr<Impl> impl_;
        PimplTestIncomplete() : impl_ (0) {}
    };

}

COMPILE_FAIL(pimpl_ptr_incomplete)
{
    PimplTestIncomplete<> ob;
}

#endif

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
