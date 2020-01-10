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
    \brief safe_bool unit tests */

#include "safe_bool.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class TestTrue : public senf::safe_bool<TestTrue>
    {
    public:
        bool boolean_test() const {
            return true;
        }
    };

    class TestFalse : public senf::safe_bool<TestFalse>
    {
    public:
        bool boolean_test() const {
            return false;
        }
    };
}

SENF_AUTO_TEST_CASE(safeBool)
{
    BOOST_CHECK( TestTrue() );
    BOOST_CHECK( ! TestFalse() );
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
