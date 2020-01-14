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
    \brief TypeInfo unit tests */

#include "TypeInfo.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace test {

    struct Base {
        virtual ~Base() {}
    };

    template <class C, int N>
    struct Foo : public Base
    {};

    enum Blub { A, B, C };
}

SENF_AUTO_TEST_CASE(prettyName)
{
    typedef test::Foo< test::Foo<test::Blub, 1>, 10> TestType;
    TestType ob;
    test::Base const & baseOb(ob);

    BOOST_CHECK_EQUAL( senf::prettyName(typeid(int)), "int");
    BOOST_CHECK_EQUAL( senf::prettyName(typeid(baseOb)), "test::Foo<test::Foo<test::Blub, 1>, 10>" );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
