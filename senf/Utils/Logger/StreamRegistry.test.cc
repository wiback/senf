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
    \brief StreamRegistry unit tests */

#include "main.test.hh"

// Custom includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(streamRegistry)
{
    char const * streams[] =
        { "senf::SenfLog", "senf::log::Debug", "senf::log::test::myStream" };

    BOOST_CHECK_EQUAL_COLLECTIONS( senf::log::StreamRegistry::instance().begin(),
                                   senf::log::StreamRegistry::instance().end(),
                                   streams, streams+sizeof(streams)/sizeof(streams[0]) );
    BOOST_CHECK_EQUAL( senf::log::detail::StreamBase::nStreams, 3u );
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
