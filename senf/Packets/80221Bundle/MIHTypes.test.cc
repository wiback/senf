// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief MIHTypes unit tests */

//#include "MIHTypes.test.hh"
//#include "MIHTypes.test.ih"

// Custom includes
#include "MIHTypes.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
using namespace senf;

BOOST_AUTO_UNIT_TEST(mihfId)
{
    MIHFId id ( MACAddress::from_string("01:02:03:04:05:06"));
    BOOST_CHECK_EQUAL( id.type(), MIHFId::MACAddress);
    BOOST_CHECK_EQUAL( id, MIHFId( MACAddress::from_string("01:02:03:04:05:06")));
    BOOST_CHECK( id != MIHFId( MACAddress::from_string("01:02:03:04:05:07")));
    BOOST_CHECK( id != MIHFId( INet4Address::from_string("128.129.130.131")));
    BOOST_CHECK( id < MIHFId( MACAddress::from_string("01:02:03:04:05:07")));
    BOOST_CHECK( id < MIHFId( INet4Address::from_string("128.129.130.131")));
}

///////////////////////////////cc.e////////////////////////////////////////
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
