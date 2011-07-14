// $Id$
//
// Copyright (C) 2010
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
    \brief pimpl_ptr.test unit tests */

//#include "pimpl_ptr.test.hh"
//#include "pimpl_ptr.test.ih"

// Custom includes
#include "pimpl_ptr.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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
