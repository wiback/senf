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
    \brief LLAddressing unit tests */

#include "LLAddressing.hh"

// Custom includes
#include <senf/Utils/String.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(llAddress)
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
