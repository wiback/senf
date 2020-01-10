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
    \brief pool_alloc_mixin unit tests */

#include "pool_alloc_mixin.hh"

// Custom includes
#include <boost/scoped_ptr.hpp>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    class Test : public senf::pool_alloc_mixin<Test>
    {};
}

SENF_AUTO_TEST_CASE(poolAllocMixin)
{
#ifdef SENF_DEBUG
    BOOST_CHECK_EQUAL( Test::allocCounter(), 0u );

    {
        boost::scoped_ptr<Test> test (new Test());
        BOOST_CHECK_EQUAL( Test::allocCounter(), 1u );
    }

    BOOST_CHECK_EQUAL( Test::allocCounter(), 0u );
#else
    BOOST_CHECK( true );
#endif
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
