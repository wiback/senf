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
    \brief pimpl_ptr.test unit tests */

#include "pimpl_ptr.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_TEST_CASE(pimpl_ptr)
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
