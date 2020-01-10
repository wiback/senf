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
    \brief MIHTypes unit tests */

#include "MIHTypes.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

SENF_AUTO_TEST_CASE(mihfId)
{
    MIHFId id ( MACAddress::from_string("01:02:03:04:05:06"));
    BOOST_CHECK_EQUAL( id.type(), MIHFId::MACAddress);
    BOOST_CHECK_EQUAL( id, MIHFId( MACAddress::from_string("01:02:03:04:05:06")));
    SENF_CHECK_NOT_EQUAL( id, MIHFId( MACAddress::from_string("01:02:03:04:05:07")));
    SENF_CHECK_NOT_EQUAL( id, MIHFId( INet4Address::from_string("128.129.130.131")));
    BOOST_CHECK( id < MIHFId( MACAddress::from_string("01:02:03:04:05:07")));
    BOOST_CHECK( id < MIHFId( INet4Address::from_string("128.129.130.131")));
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
