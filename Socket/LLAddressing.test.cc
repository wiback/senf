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
#include "LLAddressing.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(llAddress)
{
    { 
        satcom::lib::LLSocketAddress a;

        BOOST_CHECK_EQUAL( a.protocol(), 0u );
        BOOST_CHECK_EQUAL( a.interface(), "" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), 0u );
        BOOST_CHECK_EQUAL( a.address(), "" );

        a.address(satcom::lib::llAddress("05-10-1A-2f-25-30"));
        BOOST_CHECK_EQUAL( satcom::lib::llAddress(a.address()), "05-10-1a-2f-25-30" );
        a.interface("lo");
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        a.protocol(123);
        BOOST_CHECK_EQUAL( a.protocol(), 123u );
    }
    
    {
        satcom::lib::LLSocketAddress a (
            satcom::lib::llAddress("11-12-13-14-15-16"), "lo");
        
        BOOST_CHECK_EQUAL( a.protocol(), 0u );
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), 0u );
        BOOST_CHECK_EQUAL( satcom::lib::llAddress(a.address()), "11-12-13-14-15-16" );
    }

    {
        satcom::lib::LLSocketAddress a (123, "lo");
        
        BOOST_CHECK_EQUAL( a.protocol(), 123u );
        BOOST_CHECK_EQUAL( a.interface(), "lo" );
        BOOST_CHECK_EQUAL( a.arptype(), 0u );
        BOOST_CHECK_EQUAL( a.pkttype(), 0u );
        BOOST_CHECK_EQUAL( satcom::lib::llAddress(a.address()), "" );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
