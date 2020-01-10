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
    \brief AreaRegistry unit tests */

#include "main.test.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(areaRegistry)
{
    BOOST_CHECK( true );
#if 0
    // Need to rewrite this test ...
    char const * areas[] = { "GlobalTestArea",
                             "senf::log::DefaultArea",
                             "senf::log::test::Foo",
                             "senf::log::test::myArea" };

    BOOST_CHECK_EQUAL_COLLECTIONS( senf::log::AreaRegistry::instance().begin(),
                                   senf::log::AreaRegistry::instance().end(),
                                   areas, areas+sizeof(areas)/sizeof(areas[0]) );
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
