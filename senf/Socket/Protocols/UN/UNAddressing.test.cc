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
    \brief UNAddressing unit tests */

//#include "UNAddressing.test.hh"
//#include "UNAddressing.test.ih"

// Custom includes
#include "UNAddressing.hh"
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include <sstream>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(UNSocketAddress)
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
    BOOST_CHECK_THROW( senf::UNSocketAddress(std::string(1024, 'x')), senf::AddressSyntaxException );
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
