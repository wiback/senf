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
    \brief UNAddressing unit tests */

#include "UNAddressing.hh"

// Custom includes
#include <sstream>
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include <senf/Utils/IgnoreValue.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(UNSocketAddress)
{
    senf::UNSocketAddress addr;
    BOOST_CHECK( ! addr);

    std::string path ("/tmp/SENF_TEST");
    addr.path( path);

    BOOST_CHECK( addr);
    BOOST_CHECK_EQUAL( addr.path(), path);
    BOOST_CHECK_EQUAL( addr, senf::UNSocketAddress(path));
    BOOST_CHECK_EQUAL( addr, senf::UNSocketAddress(addr));

    std::stringstream str;
    str << addr;
    BOOST_CHECK_EQUAL( str.str(), path );

    // UNSocketAddress path too long
    BOOST_CHECK_THROW( senf::IGNORE(senf::UNSocketAddress(std::string(1024, 'x'))), senf::AddressSyntaxException );
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
