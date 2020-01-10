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
    \brief Config unit tests */

#include "main.test.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(logConfig)
{
    BOOST_CHECK((   senf::log::Enabled< senf::log::Debug,
                                        senf::log::DefaultArea,
                                        senf::log::NOTICE >::value ));

    BOOST_CHECK(( ! senf::log::Enabled< senf::log::Debug,
                                        senf::log::DefaultArea,
                                        senf::log::VERBOSE >::value ));

    BOOST_CHECK(( ! senf::log::Enabled< senf::log::test::myStream,
                                        senf::log::DefaultArea,
                                        senf::log::VERBOSE >::value ));


    BOOST_CHECK((   senf::log::Enabled< senf::log::test::myStream,
                                        senf::log::test::Foo,
                                        senf::log::VERBOSE >::value ));
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
