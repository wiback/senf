// $Id$
//
// Copyright (C) 2006
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief LLAddressing unit tests */

//#include "LLAddressing.test.hh"
//#include "LLAddressing.test.ih"

// Custom includes
#include "LLAddressing.hh"
#include <senf/Utils/String.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(llAddress)
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
                senf::UnknownInterfaceException );

        LLSocketAddress addr ("lo");

        BOOST_CHECK_EQUAL( addr.protocol(), 0u );
        BOOST_CHECK_EQUAL( addr.interface(), "lo" );
        BOOST_CHECK_EQUAL( addr.arptype(), 0u );
        BOOST_CHECK_EQUAL( addr.pkttype(), LLSocketAddress::Undefined );
        BOOST_CHECK( ! addr.address() );

        BOOST_CHECK_EQUAL( LLSocketAddress("").interface(), "" );
    }
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
