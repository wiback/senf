// $Id$
//
// Copyright (C) 2009
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief MIHTypes unit tests */

//#include "MIHTypes.test.hh"
//#include "MIHTypes.test.ih"

// Custom includes
#include "MIHTypes.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

SENF_AUTO_UNIT_TEST(mihfId)
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
