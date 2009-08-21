// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief LLAddressing unit tests */

//#include "LLAddressing.test.hh"
//#include "LLAddressing.test.ih"

// Custom includes
#include "../../../Utils/String.hh"
#include "LLAddressing.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(llAddress)
{
    using senf::LLSocketAddress;
    
    {
        LLSocketAddress addr;

        BOOST_CHECK( ! addr );
        BOOST_CHECK_EQUAL( addr.protocol(), 0u );
        BOOST_CHECK_EQUAL( addr.interface(), "" );
        BOOST_CHECK_EQUAL( addr.arptype(), 0u );
        BOOST_CHECK_EQUAL( addr.pkttype(), LLSocketAddress::Undefined );
        BOOST_CHECK( ! addr.address() );

        senf::MACAddress mac (senf::MACAddress::from_string("05-10-1A-2f-25-30"));
        addr.address( mac);
        BOOST_CHECK( addr );
        BOOST_CHECK_EQUAL( addr, LLSocketAddress(mac));
        BOOST_CHECK_EQUAL( addr, LLSocketAddress(addr));
        BOOST_CHECK_EQUAL( addr.address(), mac );
        addr.interface("lo");
        BOOST_CHECK_EQUAL( addr.interface(), "lo" );
        addr.protocol(123);
        BOOST_CHECK_EQUAL( addr.protocol(), 123u );
    }

    {
        LLSocketAddress addr (
            senf::MACAddress::from_string("11-12-13-14-15-16"), "lo");

        BOOST_CHECK_EQUAL( addr.protocol(), 0u );
        BOOST_CHECK_EQUAL( addr.interface(), "lo" );
        BOOST_CHECK_EQUAL( addr.arptype(), 0u );
        BOOST_CHECK_EQUAL( addr.pkttype(), LLSocketAddress::Undefined );
        BOOST_CHECK_EQUAL( senf::str(addr.address()), "11:12:13:14:15:16" );
    }

    {
        LLSocketAddress addr (123, "lo");

        BOOST_CHECK_EQUAL( addr.protocol(), 123u );
        BOOST_CHECK_EQUAL( addr.interface(), "lo" );
        BOOST_CHECK_EQUAL( addr.arptype(), 0u );
        BOOST_CHECK_EQUAL( addr.pkttype(), LLSocketAddress::Undefined );
        BOOST_CHECK( ! addr.address() );
    }
    
    {
        BOOST_CHECK_THROW( LLSocketAddress addr("SENF_TEST_INVALID_INTERFACENAME"),
                senf::AddressSyntaxException );
        
        LLSocketAddress addr ("lo");
        
        BOOST_CHECK_EQUAL( addr.protocol(), 0u );
        BOOST_CHECK_EQUAL( addr.interface(), "lo" );
        BOOST_CHECK_EQUAL( addr.arptype(), 0u );
        BOOST_CHECK_EQUAL( addr.pkttype(), LLSocketAddress::Undefined );
        BOOST_CHECK( ! addr.address() );
        
        BOOST_CHECK_EQUAL( LLSocketAddress("").interface(), "" );
    }
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
