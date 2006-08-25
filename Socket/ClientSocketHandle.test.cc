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

//#include "ClientSocketHandle.test.hh"
//#include "ClientSocketHandle.test.ih"

// Custom includes
#include "SocketPolicy.test.hh"
#include "SocketProtocol.test.hh"
#include "ClientSocketHandle.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    namespace sl = satcom::lib;

    class MySocketHandle
        : public sl::ClientSocketHandle<sl::test::SomeProtocol::Policy>
    {
    public:
        MySocketHandle()
            : sl::ClientSocketHandle<sl::test::SomeProtocol::Policy>(
                std::auto_ptr<sl::SocketProtocol>(new sl::test::SomeProtocol()))
            {}
    };
}

BOOST_AUTO_UNIT_TEST(clientSocketHandle)
{
    MySocketHandle myh;

    // conversion to other socket handles
    {
        typedef sl::MakeSocketPolicy<
            sl::test::SomeFramingPolicy,
            sl::test::SomeReadPolicy,
            sl::test::SomeWritePolicy
            >::policy OtherSocketPolicy;
        typedef sl::SocketHandle<OtherSocketPolicy> OtherSocketHandle;
    
        OtherSocketHandle ssh (myh);
        ssh = myh;
    }

    // reading and writing
    BOOST_CHECK_EQUAL( myh.read(), "TEST-READ" );
    {
        std::string buf("FOO-BAR");
        myh.read(buf);
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        ::strcpy(buf,"0123456789");
        BOOST_CHECK_EQUAL( myh.read(buf,10), 9u );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    BOOST_CHECK_EQUAL( myh.readfrom().first, "TEST-READ" );
    {
        std::string buf("FOO-BAR");
        satcom::lib::nil addr;
        myh.readfrom(buf,addr);
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        satcom::lib::nil addr;
        ::strcpy(buf,"0123456789");
        BOOST_CHECK_EQUAL( myh.readfrom(buf,10,addr), 9u );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    BOOST_CHECK_EQUAL( myh.write("TEST-WRITE"), 10u );
    BOOST_CHECK_EQUAL( myh.write("TEST"), 0u );
    BOOST_CHECK_EQUAL( myh.write("TEST-WRITE9",10), 10u );
    BOOST_CHECK_EQUAL( myh.writeto(satcom::lib::nil(),"TEST-WRITE"), 10u );
    BOOST_CHECK_EQUAL( myh.writeto(satcom::lib::nil(),"TEST-WRITE9",10), 10u );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
