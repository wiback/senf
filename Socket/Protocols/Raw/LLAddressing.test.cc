// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Unit tests

//#include "LLAddressing.test.hh"
//#include "LLAddressing.test.ih"

// Custom includes
#include <boost/lexical_cast.hpp>
#include "LLAddressing.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(llAddress)
{
    {
        senf::LLSocketAddress a;

        BOOST_CHECK_EQUAL( a.protocol(), 0u );
        BOOST_CHECK_EQUAL( a.interface(), "" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), senf::LLSocketAddress::Undefined );
        BOOST_CHECK( ! a.address() );

        a.address(senf::MACAddress::from_string("05-10-1A-2f-25-30"));
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(a.address()), "05:10:1a:2f:25:30" );
        a.interface("lo");
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        a.protocol(123);
        BOOST_CHECK_EQUAL( a.protocol(), 123u );
    }

    {
        senf::LLSocketAddress a (
            senf::MACAddress::from_string("11-12-13-14-15-16"), "lo");

        BOOST_CHECK_EQUAL( a.protocol(), 0u );
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), senf::LLSocketAddress::Undefined );
        BOOST_CHECK_EQUAL( boost::lexical_cast<std::string>(a.address()), "11:12:13:14:15:16" );
    }

    {
        senf::LLSocketAddress a (123, "lo");

        BOOST_CHECK_EQUAL( a.protocol(), 123u );
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), senf::LLSocketAddress::Undefined );
        BOOST_CHECK( ! a.address() );
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
